module My_query = [%graphql7
  {|
  query ($arg: NonrecursiveInput!) {
    nonrecursiveInput(arg: $arg) # comment to test
  }
|}
];
