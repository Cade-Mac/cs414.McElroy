open Zipper

let failures = ref 0

let check name actual expected =
  if actual = expected then Printf.printf "  PASS: %s\n" name
  else begin
    Printf.printf "  FAIL: %s\n" name;
    incr failures
  end

let show_int_option = function
  | None -> "None"
  | Some x -> string_of_int x

let show z =
  let items = String.concat "; " (List.map string_of_int (to_list z)) in
  Printf.printf "  list = [%s], focus = %s, front = %s, back = %s\n" items
    (show_int_option (focus z))
    (show_int_option (front z))
    (show_int_option (back z))

let () =
  print_endline "Test 1: empty zipper";
  let z = create () in
  check "empty is true" (empty z) true;
  check "focus is None" (focus z) None;
  check "front is None" (front z) None;
  check "back is None" (back z) None;
  check "move_left on empty stays empty" (empty (move_left z)) true;
  check "move_right on empty stays empty" (empty (move_right z)) true;

  print_endline "\nTest 2: one item";
  let z = push_back 7 (create ()) in
  show z;
  check "empty is false" (empty z) false;
  check "focus is 7" (focus z) (Some 7);
  check "front is 7" (front z) (Some 7);
  check "back is 7" (back z) (Some 7);

  print_endline "\nTest 3: push_back builds the list in order";
  let z = create () |> push_back 1 |> push_back 2 |> push_back 3 in
  show z;
  check "list is [1; 2; 3]" (to_list z) [ 1; 2; 3 ];
  check "focus stays on first item" (focus z) (Some 1);
  check "front is 1" (front z) (Some 1);
  check "back is 3" (back z) (Some 3);

  print_endline "\nTest 4: push_front adds to the left end";
  let z = push_front 0 z in
  show z;
  check "list is [0; 1; 2; 3]" (to_list z) [ 0; 1; 2; 3 ];
  check "front is 0" (front z) (Some 0);
  check "focus did not move" (focus z) (Some 1);

  print_endline "\nTest 5: moving the focus";
  let z = of_list [ 10; 20; 30; 40; 50 ] in
  show z;
  let z = move_right z in
  check "move_right -> 20" (focus z) (Some 20);
  let z = move_right (move_right z) in
  check "move_right twice -> 40" (focus z) (Some 40);
  show z;
  let z = move_left z in
  check "move_left -> 30" (focus z) (Some 30);
  check "list order unchanged" (to_list z) [ 10; 20; 30; 40; 50 ];
  check "front still 10" (front z) (Some 10);
  check "back still 50" (back z) (Some 50);

  print_endline "\nTest 6: moving past the ends does nothing";
  let z = of_list [ 1; 2; 3 ] in
  check "move_left at left end stays on 1" (focus (move_left z)) (Some 1);
  let right_end = move_right (move_right z) in
  check "reached right end" (focus right_end) (Some 3);
  check "move_right at right end stays on 3" (focus (move_right right_end)) (Some 3);

  print_endline "\nTest 7: pushing while focus is in the middle";
  let z = of_list [ 2; 3; 4 ] |> move_right in
  let z = z |> push_front 1 |> push_back 5 in
  show z;
  check "list is [1; 2; 3; 4; 5]" (to_list z) [ 1; 2; 3; 4; 5 ];
  check "focus still 3" (focus z) (Some 3);
  check "front is 1" (front z) (Some 1);
  check "back is 5" (back z) (Some 5);

  print_endline "\nTest 8: the type works for any element type";
  let words = of_list [ "alpha"; "beta"; "gamma" ] |> move_right in
  check "string focus" (focus words) (Some "beta");
  check "string front" (front words) (Some "alpha");
  check "string back" (back words) (Some "gamma");

  print_endline "\nTest 9: old versions are not changed";
  let original = of_list [ 1; 2; 3 ] in
  let moved = move_right original in
  check "original focus still 1" (focus original) (Some 1);
  check "new version focus is 2" (focus moved) (Some 2);

  print_newline ();
  if !failures = 0 then print_endline "All tests passed."
  else print_endline "Some tests failed.";
  exit (if !failures = 0 then 0 else 1)
