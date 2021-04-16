module MyQuery = [%graphql7
  {|
  query ($query: String!) {
    argNamedQuery(query: $query)
  }
|}
];

Jest.(
  describe("Argument named 'query'", () => {
    open Expect;
    open! Expect.Operators;

    test("Serializes variables", () =>
      expect(MyQuery.make(~query="a query", ())##variables)
      == Js.Json.parseExn({|{"query": "a query"}|})
    );

    test("Can create variables", () =>
      expect(MyQuery.makeVariables(~query="a query", ()))
      == Js.Json.parseExn({|{"query": "a query"}|})
    );

    test("No name clash with the query field", () =>
      expect(MyQuery.make(~query="a query", ())##query) != "a query"
    );
  })
);
