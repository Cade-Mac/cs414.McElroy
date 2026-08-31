type 'a rose = Node of 'a * 'a rose list

(* 1. size: Returns the total number of nodes in the rose tree *)
let rec size : 'a rose -> int = fun (Node (_, children)) ->
  1 + List.fold_left (fun acc child -> acc + size child) 0 children

(* 2. map: Applies function f to every node value in the rose tree *)
let rec map : ('a -> 'b) -> 'a rose -> 'b rose = fun f (Node (value, children)) ->
  Node (f value, List.map (map f) children)

(* 3. fold: Combines node values and child results into a summary value *)
let rec fold : ('a -> 'b list -> 'b) -> 'a rose -> 'b = fun f (Node (value, children)) ->
  let child_results = List.map (fold f) children in
  f value child_results

let () =
  let sample_rose =
    Node (1, [
      Node (2, []);
      Node (3, [Node (5, []); Node (6, [])]);
      Node (4, [])
    ])
  in

  print_endline "=== QUESTION 01 TESTS ===";

  (* Calls size *)
  print_endline ("Size of tree: " ^ string_of_int (size sample_rose));

  (* Calls map *)
  let doubled = map (fun x -> x * 2) sample_rose in
  print_endline ("Size of mapped tree: " ^ string_of_int (size doubled));

  (* Calls fold *)
  let total_sum = fold (fun v child_sums -> v + List.fold_left (+) 0 child_sums) sample_rose in
  print_endline ("Sum of tree nodes via fold: " ^ string_of_int total_sum)
