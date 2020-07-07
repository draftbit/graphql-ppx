[@ocaml.ppx.context
  {
    tool_name: "migrate_driver",
    include_dirs: [],
    load_path: [],
    open_modules: [],
    for_package: None,
    debug: false,
    use_threads: false,
    use_vmthreads: false,
    recursive_types: false,
    principal: false,
    transparent_modules: false,
    unboxed_types: false,
    unsafe_string: false,
    cookies: [],
  }
];
module MyQuery = {
  [@ocaml.warning "-32"];
  module Raw = {
    type t = {recursiveInput: string};
    type t_variables = {arg: t_variables_RecursiveInput}
    and t_variables_RecursiveInput = {
      otherField: Js.Nullable.t(string),
      inner: Js.Nullable.t(t_variables_RecursiveInput),
      enum: Js.Nullable.t(string),
    };
  };
  type t = {recursiveInput: string};
  /**The GraphQL query string*/
  let query = [%raw
    "require(\"gql\")`\n  query ($arg: RecursiveInput!)  {\n    recursiveInput(arg: $arg)\n  }\n`"
  ];
  type t_variables = {arg: t_variables_RecursiveInput}
  and t_variables_RecursiveInput = {
    otherField: option(string),
    inner: option(t_variables_RecursiveInput),
    enum: option([ | `FIRST | `SECOND | `THIRD]),
  };
  /**Parse the JSON GraphQL data to ReasonML data types*/
  let parse = (value: Raw.t): t => (
    {
      recursiveInput: {
        let value = (value: Raw.t).recursiveInput;
        value;
      },
    }: t
  );
  /**Serialize the ReasonML GraphQL data that was parsed using the parse function back to the original JSON compatible data */
  let serialize = (value: t): Raw.t => (
    {
      let recursiveInput = {
        let value = (value: t).recursiveInput;
        value;
      };
      {recursiveInput: recursiveInput};
    }: Raw.t
  );
  let rec serializeVariables: t_variables => Raw.t_variables =
    inp => {
      arg:
        (a => serializeInputObjectRecursiveInput(a))((inp: t_variables).arg),
    }
  and serializeInputObjectRecursiveInput:
    t_variables_RecursiveInput => Raw.t_variables_RecursiveInput =
    inp => {
      otherField:
        (
          a =>
            switch (a) {
            | None => Js.Nullable.undefined
            | Some(b) => Js.Nullable.return((a => a)(b))
            }
        )(
          (inp: t_variables_RecursiveInput).otherField,
        ),
      inner:
        (
          a =>
            switch (a) {
            | None => Js.Nullable.undefined
            | Some(b) =>
              Js.Nullable.return(
                (a => serializeInputObjectRecursiveInput(a))(b),
              )
            }
        )(
          (inp: t_variables_RecursiveInput).inner,
        ),
      enum:
        (
          a =>
            switch (a) {
            | None => Js.Nullable.undefined
            | Some(b) =>
              Js.Nullable.return(
                (
                  a =>
                    switch (a) {
                    | `FIRST => "FIRST"
                    | `SECOND => "SECOND"
                    | `THIRD => "THIRD"
                    }
                )(
                  b,
                ),
              )
            }
        )(
          (inp: t_variables_RecursiveInput).enum,
        ),
    };
  let makeVariables = (~arg, ()) =>
    serializeVariables({arg: arg}: t_variables)
  and makeInputObjectRecursiveInput =
      (~otherField=?, ~inner=?, ~enum=?, ()): t_variables_RecursiveInput => {
    otherField,
    inner,
    enum,
  };
  external unsafe_fromJson: Js.Json.t => Raw.t = "%identity";
  external toJson: Raw.t => Js.Json.t = "%identity";
  external variablesToJson: Raw.t_variables => Js.Json.t = "%identity";
  module Z__INTERNAL = {
    type nonrec _graphql_arg_90;
    /**Variable **$arg** has the following graphql type:

```
RecursiveInput {
  otherField: String
  inner: RecursiveInput
  enum: SampleField
}!
```*/
    let _graphql_arg_90: _graphql_arg_90 = Obj.magic(0);
    type nonrec _graphql_arg_85;
    /**Argument **arg** on field **recursiveInput** has the following graphql type:

```
RecursiveInput {
  otherField: String
  inner: RecursiveInput
  enum: SampleField
}!
```*/
    let _graphql_arg_85: _graphql_arg_85 = Obj.magic(0);
    type root = t;
    type nonrec _graphql_RecursiveInput_47;
    /**```
RecursiveInput {
  otherField: String
  inner: RecursiveInput
  enum: SampleField
}
```*/
    let _graphql_RecursiveInput_47: _graphql_RecursiveInput_47 = Obj.magic(0);
    type nonrec graphql_module;
    /****--- graphql-ppx module ---**

The contents of this module are automatically generated by `graphql-ppx`.
The following is simply an overview of the most important variables and types that you can access from this module.

```
module MyQuery {
  /**
  The GraphQL query string
  */
  let query: string;

  /**
  This is the main type of the result you will get back.
  You can hover above the identifier key (e.g. query or mutation) to see the fully generated type for your module.
  */
  type t;

  /**
  Parse the JSON GraphQL data to ReasonML data types
  */
  let parse: Raw.t => t;

  /**
  Serialize the ReasonML GraphQL data that was parsed using the parse function back to the original JSON compatible data
  */
  let serialize: t => Raw.t;

  /**
  This is the JSON compatible type of the GraphQL data.
  It should not be necessary to access the types inside for normal use cases.
  */
  module Raw: { type t; };
}
```*/
    let graphql_module: graphql_module = Obj.magic(0);
  };
};
