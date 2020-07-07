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
module StringOfInt = {
  let parse = string_of_int;
  let serialize = int_of_string;
  type t = string;
};
module IntOfString = {
  let parse = int_of_string;
  let serialize = string_of_int;
  type t = int;
};
module MyQuery = {
  [@ocaml.warning "-32"];
  module Raw = {
    type t_variousScalars = {
      string,
      int,
    };
    type t = {variousScalars: t_variousScalars};
    type t_variables = unit;
  };
  type t_variousScalars = {
    string: IntOfString.t,
    int: StringOfInt.t,
  };
  type t = {variousScalars: t_variousScalars};
  /**The GraphQL query string*/
  let query = "query   {\nvariousScalars  {\nstring  \nint  \n}\n\n}\n";
  type t_variables = unit;
  /**Parse the JSON GraphQL data to ReasonML data types*/
  let parse = (value: Raw.t): t => (
    {
      variousScalars: {
        let value = (value: Raw.t).variousScalars;
        (
          {
            string: {
              let value = (value: Raw.t_variousScalars).string;
              IntOfString.parse(value);
            },
            int: {
              let value = (value: Raw.t_variousScalars).int;
              StringOfInt.parse(value);
            },
          }: t_variousScalars
        );
      },
    }: t
  );
  /**Serialize the ReasonML GraphQL data that was parsed using the parse function back to the original JSON compatible data */
  let serialize = (value: t): Raw.t => (
    {
      let variousScalars = {
        let value = (value: t).variousScalars;
        (
          {
            let int = {
              let value = (value: t_variousScalars).int;
              StringOfInt.serialize(value);
            }
            and string = {
              let value = (value: t_variousScalars).string;
              IntOfString.serialize(value);
            };
            {string, int};
          }: Raw.t_variousScalars
        );
      };
      {variousScalars: variousScalars};
    }: Raw.t
  );
  let serializeVariables = () => ();
  let makeVariables = () => ();
  let makeDefaultVariables = () => makeVariables();
  external unsafe_fromJson: Js.Json.t => Raw.t = "%identity";
  external toJson: Raw.t => Js.Json.t = "%identity";
  external variablesToJson: Raw.t_variables => Js.Json.t = "%identity";
  module Z__INTERNAL = {
    type root = t;
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
