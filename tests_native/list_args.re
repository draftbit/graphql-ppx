open Test_shared;

module MyQuery = [%graphql7
  {|
  query (
    $nullableOfNullable: [String],
    $nullableOfNonNullable: [String!],
    $nonNullableOfNullable: [String]!,
    $nonNullableOfNonNullable: [String!]!,
  ) {
    listsInput(arg: {
      nullableOfNullable: $nullableOfNullable,
      nullableOfNonNullable: $nullableOfNonNullable,
      nonNullableOfNullable: $nonNullableOfNullable,
      nonNullableOfNonNullable: $nonNullableOfNonNullable,
    })
  }
|}
];

type qt = {. lists: string};

let my_query: module Alcotest.TESTABLE with type t = qt =
  (module
   {
     type t = qt;

     let pp = (formatter, obj: qt) =>
       Format.fprintf(formatter, "< lists = @[%s@] >", obj#lists);

     let equal = (a: qt, b: qt) => a#lists == b#lists;
   });

let omit_nullable_args = () =>
  test_json(
    MyQuery.make(
      ~nonNullableOfNullable=[||],
      ~nonNullableOfNonNullable=[||],
      (),
    )#
      variables,
    Yojson.Basic.from_string(
      {| {
      "nullableOfNullable": null,
      "nullableOfNonNullable": null,
      "nonNullableOfNullable": [],
      "nonNullableOfNonNullable": []
    } |},
    ),
  );

let allows_none_in_lists = () =>
  test_json(
    MyQuery.make(
      ~nullableOfNullable=[|Some("x"), None, Some("y")|],
      ~nonNullableOfNullable=[|Some("a"), None, Some("b")|],
      ~nonNullableOfNonNullable=[|"1", "2", "3"|],
      (),
    )#
      variables,
    Yojson.Basic.from_string(
      {| {
      "nullableOfNullable": ["x", null, "y"],
      "nullableOfNonNullable": null,
      "nonNullableOfNullable": ["a", null, "b"],
      "nonNullableOfNonNullable": ["1", "2", "3"]
    } |},
    ),
  );

let tests = [
  ("Can omit nullable arguments", `Quick, omit_nullable_args),
  ("Allows None in lists with nullable items", `Quick, allows_none_in_lists),
];
