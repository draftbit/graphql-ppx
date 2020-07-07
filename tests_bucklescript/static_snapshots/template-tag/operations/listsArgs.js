// Generated by BUCKLESCRIPT, PLEASE EDIT WITH CARE
'use strict';

var $$Array = require("bs-platform/lib/js/array.js");

var Raw = { };

var query = (require("gql")`
  query ($nullableOfNullable: [String], $nullableOfNonNullable: [String!], $nonNullableOfNullable: [String]!, $nonNullableOfNonNullable: [String!]!)  {
  listsInput(arg: {nullableOfNullable: $nullableOfNullable, nullableOfNonNullable: $nullableOfNonNullable, nonNullableOfNullable: $nonNullableOfNullable, nonNullableOfNonNullable: $nonNullableOfNonNullable})
  }
`);

function parse(value) {
  return {
          listsInput: value.listsInput
        };
}

function serialize(value) {
  var value$1 = value.listsInput;
  return {
          listsInput: value$1
        };
}

function serializeVariables(inp) {
  var a = inp.nullableOfNullable;
  var a$1 = inp.nullableOfNonNullable;
  var a$2 = inp.nonNullableOfNullable;
  var a$3 = inp.nonNullableOfNonNullable;
  return {
          nullableOfNullable: a !== undefined ? $$Array.map((function (b) {
                    if (b !== undefined) {
                      return b;
                    }
                    
                  }), a) : undefined,
          nullableOfNonNullable: a$1 !== undefined ? $$Array.map((function (b) {
                    return b;
                  }), a$1) : undefined,
          nonNullableOfNullable: $$Array.map((function (b) {
                  if (b !== undefined) {
                    return b;
                  }
                  
                }), a$2),
          nonNullableOfNonNullable: $$Array.map((function (b) {
                  return b;
                }), a$3)
        };
}

function makeVariables(nullableOfNullable, nullableOfNonNullable, nonNullableOfNullable, nonNullableOfNonNullable, param) {
  return serializeVariables({
              nullableOfNullable: nullableOfNullable,
              nullableOfNonNullable: nullableOfNonNullable,
              nonNullableOfNullable: nonNullableOfNullable,
              nonNullableOfNonNullable: nonNullableOfNonNullable
            });
}

var Z__INTERNAL = {
  _graphql_arg_219: 0,
  graphql_module: 0
};

var MyQuery = {
  Raw: Raw,
  query: query,
  parse: parse,
  serialize: serialize,
  serializeVariables: serializeVariables,
  makeVariables: makeVariables,
  Z__INTERNAL: Z__INTERNAL
};

exports.MyQuery = MyQuery;
/* query Not a pure module */
