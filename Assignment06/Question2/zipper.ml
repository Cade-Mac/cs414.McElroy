(* A singly linked list, built from scratch instead of using OCaml's list. *)
type 'a node =
  | Nil
  | Cons of 'a * 'a node

(* A zipper list is either empty, or it has a focus item with a list on
   each side of it.

   Zipper (front, focus, back)

   - front holds the items to the left of the focus, stored nearest-first.
     The item right next to the focus is at the head of front, and the very
     first item of the whole list is at the end of front.
   - back holds the items to the right of the focus, also nearest-first.

   Example: the list [1; 2; 3; 4; 5] with focus on 3 is stored as
     Zipper (Cons (2, Cons (1, Nil)), 3, Cons (4, Cons (5, Nil)))

   Storing front in reverse is what makes move_left and move_right cheap:
   every move only takes the head off one side and puts it on the other. *)
  
type 'a zipper =
  | Empty
  | Zipper of 'a node * 'a * 'a node

let rec node_append_end item = function
  | Nil -> Cons (item, Nil)
  | Cons (x, rest) -> Cons (x, node_append_end item rest)

let rec node_last = function
  | Nil -> None
  | Cons (x, Nil) -> Some x
  | Cons (_, rest) -> node_last rest

let node_rev nodes =
  let rec go acc = function
    | Nil -> acc
    | Cons (x, rest) -> go (Cons (x, acc)) rest
  in
  go Nil nodes

let rec node_to_list = function
  | Nil -> []
  | Cons (x, rest) -> x :: node_to_list rest

let create () = Empty

let empty = function
  | Empty -> true
  | Zipper _ -> false

let move_left = function
  | Empty -> Empty
  | Zipper (Nil, _, _) as z -> z
  | Zipper (Cons (left, front), focus, back) ->
      Zipper (front, left, Cons (focus, back))

let move_right = function
  | Empty -> Empty
  | Zipper (_, _, Nil) as z -> z
  | Zipper (front, focus, Cons (right, back)) ->
      Zipper (Cons (focus, front), right, back)

let push_front item = function
  | Empty -> Zipper (Nil, item, Nil)
  | Zipper (front, focus, back) -> Zipper (node_append_end item front, focus, back)

let push_back item = function
  | Empty -> Zipper (Nil, item, Nil)
  | Zipper (front, focus, back) -> Zipper (front, focus, node_append_end item back)

let front = function
  | Empty -> None
  | Zipper (Nil, focus, _) -> Some focus
  | Zipper (front, _, _) -> node_last front

let back = function
  | Empty -> None
  | Zipper (_, focus, Nil) -> Some focus
  | Zipper (_, _, back) -> node_last back

let focus = function
  | Empty -> None
  | Zipper (_, focus, _) -> Some focus

let to_list = function
  | Empty -> []
  | Zipper (front, focus, back) ->
      node_to_list (node_rev front) @ [ focus ] @ node_to_list back

let of_list items = List.fold_left (fun z x -> push_back x z) Empty items
