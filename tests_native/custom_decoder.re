module My_query = [%graphql7
  {|
  {
    variousScalars {
      string @bsDecoder(fn: "int_of_string")
      int @bsDecoder(fn: "string_of_int")
    }
  }
|}
];

type qt = {
  .
  variousScalars: {
    .
    string: int,
    int: string,
  },
};

let my_query: module Alcotest.TESTABLE with type t = qt =
  (module
   {
     type t = qt;

     let pp = (formatter, obj) =>
       Format.fprintf(
         formatter,
         "<variousScalars = @[<>string = @[%i@]; int = @[%s@]>@] >",
         obj#variousScalars#string,
         obj#variousScalars#int,
       );

     let equal = (a, b) =>
       a#variousScalars#string == b#variousScalars#string
       && a#variousScalars#int == b#variousScalars#int;
   });

let runs_the_decoder = () =>
  Alcotest.check(
    my_query,
    "query result equality",
    My_query.parse(
      Yojson.Basic.from_string(
        {|{"variousScalars": {"string": "123", "int": 456}}|},
      ),
    ),
    {as _; pub variousScalars = {as _; pub string = 123; pub int = "456"}},
  );

let tests = [("Runs the decoder", `Quick, runs_the_decoder)];
