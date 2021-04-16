open Migrate_parsetree;
open Graphql_ppx_base;
open Source_pos;

open Output_native_utils;

let argv = Sys.argv |> Array.to_list;

let add_pos = (delimLength, base, pos) => {
  pos_fname: base.pos_fname,
  pos_lnum: base.pos_lnum + pos.line,
  pos_bol: 0,
  pos_cnum:
    if (pos.line == 0) {
      delimLength + pos.col;
    } else {
      pos.col;
    },
};

let add_loc = (delimLength, base, span) => {
  loc_start: add_pos(delimLength, base.loc_start, fst(span)),
  loc_end: add_pos(delimLength, base.loc_start, snd(span)),
  loc_ghost: false,
};

let fmt_lex_err = err =>
  Graphql_lexer.(
    switch (err) {
    | Unknown_character(ch) => Printf.sprintf("Unknown character %c", ch)
    | Unexpected_character(ch) =>
      Printf.sprintf("Unexpected character %c", ch)
    | Unterminated_string => Printf.sprintf("Unterminated string literal")
    | Unknown_character_in_string(ch) =>
      Printf.sprintf("Unknown character in string literal: %c", ch)
    | Unknown_escape_sequence(s) =>
      Printf.sprintf("Unknown escape sequence in string literal: %s", s)
    | Unexpected_end_of_file => Printf.sprintf("Unexpected end of query")
    | Invalid_number => Printf.sprintf("Invalid number")
    }
  );

let fmt_parse_err = err =>
  Graphql_parser.(
    switch (err) {
    | Unexpected_token(t) =>
      Printf.sprintf("Unexpected token %s", Graphql_lexer.string_of_token(t))
    | Unexpected_end_of_file => "Unexpected end of query"
    | Lexer_error(err) => fmt_lex_err(err)
    }
  );

let is_prefixed = (prefix, str) => {
  let i = 0;
  let len = String.length(prefix);
  let j = ref(0);
  while (j^ < len
         && String.unsafe_get(prefix, j^) == String.unsafe_get(str, i + j^)) {
    incr(j);
  };
  j^ == len;
};

let make_error_expr = (loc, message) => {
  open Ast_408;
  let ext = Ast_mapper.extension_of_error(Location.error(~loc, message));
  Ast_helper.Exp.extension(~loc, ext);
};

let drop_prefix = (prefix, str) => {
  let len = String.length(prefix);
  let rest = String.length(str) - len;
  String.sub(str, len, rest);
};

let rewrite_query = (~schema=?, ~loc, ~delim, ~query, ()) => {
  open Ast_408;

  let lexer = Graphql_lexer.make(query);
  let delimLength =
    switch (delim) {
    | Some(s) => 2 + String.length(s)
    | None => 1
    };
  switch (Graphql_lexer.consume(lexer)) {
  | Result.Error(e) =>
    raise(
      Location.Error(
        Location.error(
          ~loc=add_loc(delimLength, loc, e.span) |> conv_loc,
          fmt_lex_err(e.item),
        ),
      ),
    )
  | Result.Ok(tokens) =>
    let parser = Graphql_parser.make(tokens);
    switch (Graphql_parser_document.parse_document(parser)) {
    | Result.Error(e) =>
      raise(
        Location.Error(
          Location.error(
            ~loc=add_loc(delimLength, loc, e.span) |> conv_loc,
            fmt_parse_err(e.item),
          ),
        ),
      )
    | Result.Ok(document) =>
      let config = {
        Generator_utils.map_loc: add_loc(delimLength, loc),
        delimiter: delim,
        full_document: document,
        /*  the only call site of schema, make it lazy! */
        schema: Lazy.force(Read_schema.get_schema(schema)),
      };
      switch (Validations.run_validators(config, document)) {
      | Some(errs) =>
        Ast_helper.Mod.mk(
          Pmod_structure(
            List.map(
              ((loc, msg)) => {
                let loc = conv_loc(loc);
                %stri
                [%e make_error_expr(loc, msg)];
              },
              errs,
            ),
          ),
        )
      | None =>
        let parts = Result_decoder.unify_document_schema(config, document);
        Output_native_module.generate_modules(config, parts);
      };
    };
  };
};

let extract_schema_from_config = config_fields => {
  open Ast_408;
  open Asttypes;
  open Parsetree;

  let maybe_schema_field =
    try(
      Some(
        List.find(
          config_field =>
            switch (config_field) {
            | (
                {txt: Longident.Lident("schema"), _},
                {pexp_desc: Pexp_constant(Pconst_string(_, _)), _},
              ) =>
              true
            | _ => false
            },
          config_fields,
        ),
      )
    ) {
    | _ => None
    };

  switch (maybe_schema_field) {
  | Some((_, {pexp_desc: Pexp_constant(Pconst_string(schema_name, _)), _})) =>
    Some(schema_name)
  | _ => None
  };
};

// Default configuration
let () =
  Ppx_config.(
    set_config({
      verbose_logging: false,
      output_mode: Ppx_config.String,
      verbose_error_handling:
        switch (Sys.getenv("NODE_ENV")) {
        | "production" => false
        | _ => true
        | exception Not_found => true
        },
      apollo_mode: false,
      schema_file: "graphql_schema.json",
      root_directory: Sys.getcwd(),
      raise_error_with_loc: (loc, message) => {
        let loc = conv_loc(loc);
        raise(Location.Error(Location.error(~loc, message)));
      },
      lean_parse: false,
    })
  );

let mapper = (_config, _cookies) => {
  Ast_408.(
    Ast_mapper.(
      Parsetree.(
        Asttypes.{
          ...default_mapper,
          module_expr: (mapper, mexpr) =>
            switch (mexpr) {
            | {pmod_desc: Pmod_extension(({txt: "graphql7", loc}, pstr)), _} =>
              switch (pstr) {
              | PStr([
                  {
                    pstr_desc:
                      Pstr_eval(
                        {
                          pexp_loc: loc,
                          pexp_desc:
                            Pexp_constant(Pconst_string(query, delim)),
                          _,
                        },
                        _,
                      ),
                    _,
                  },
                  {
                    pstr_desc:
                      Pstr_eval(
                        {pexp_desc: Pexp_record(fields, None), _},
                        _,
                      ),
                    _,
                  },
                ]) =>
                let maybe_schema = extract_schema_from_config(fields);
                rewrite_query(
                  ~schema=?maybe_schema,
                  ~loc=conv_loc_from_ast(loc),
                  ~delim,
                  ~query,
                  (),
                );
              | PStr([
                  {
                    pstr_desc:
                      Pstr_eval(
                        {
                          pexp_loc: loc,
                          pexp_desc:
                            Pexp_constant(Pconst_string(query, delim)),
                          _,
                        },
                        _,
                      ),
                    _,
                  },
                ]) =>
                rewrite_query(
                  ~loc=conv_loc_from_ast(loc),
                  ~delim,
                  ~query,
                  (),
                )
              | _ =>
                raise(
                  Location.Error(
                    Location.error(
                      ~loc,
                      "[%graphql7] accepts a string, e.g. [%graphql7 {| { query |}]",
                    ),
                  ),
                )
              }
            | other => default_mapper.module_expr(mapper, other)
            },
        }
      )
    )
  );
};

let args = [
  (
    "-verbose",
    Arg.Unit(
      () =>
        Ppx_config.update_config(current =>
          {...current, verbose_logging: true}
        ),
    ),
    "Defines if logging should be verbose or not",
  ),
  (
    "-apollo-mode",
    Arg.Unit(
      () =>
        Ppx_config.update_config(current => {...current, apollo_mode: true}),
    ),
    "Defines if apply Apollo specific code generation",
  ),
  (
    "-schema",
    Arg.String(
      schema_file =>
        Ppx_config.update_config(current => {...current, schema_file}),
    ),
    "<path/to/schema.json>",
  ),
  (
    "-ast-out",
    Arg.Bool(
      ast_out =>
        Ppx_config.update_config(current =>
          {
            ...current,
            output_mode: ast_out ? Ppx_config.Apollo_AST : Ppx_config.String,
          }
        ),
    ),
    "Defines if output string or AST",
  ),
  (
    "-o",
    Arg.Unit(
      () =>
        Ppx_config.update_config(current =>
          {...current, verbose_error_handling: true}
        ),
    ),
    "Verbose error handling. If not defined NODE_ENV will be used",
  ),
];

let () =
  Migrate_parsetree.(
    Driver.register(~name="graphql7", ~args, Versions.ocaml_408, mapper)
  );
