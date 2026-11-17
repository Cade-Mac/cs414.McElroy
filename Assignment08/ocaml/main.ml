(* Contract: the callback must not return or store the channel. The channel
   is only valid inside the protected scope and is closed when it ends. *)
let with_input_file path f =
  let ch = open_in path in
  Fun.protect
    ~finally:(fun () ->
      prerr_endline ("[with_input_file] closing " ^ path);
      close_in_noerr ch)
    (fun () ->
      prerr_endline ("[with_input_file] opened " ^ path);
      f ch)

let contains line needle =
  let n = String.length needle in
  let m = String.length line in
  let rec check i =
    i + n <= m && (String.sub line i n = needle || check (i + 1))
  in
  check 0

let count_matches_ocaml path needle ?(fail_after_first_line = false) () =
  if needle = "" then invalid_arg "search string must not be empty";
  with_input_file path (fun ch ->
    let rec loop count =
      match input_line ch with
      | line ->
          let count = if contains line needle then count + 1 else count in
          if fail_after_first_line then failwith "simulated failure";
          loop count
      | exception End_of_file -> count
    in
    loop 0)

let with_two_input_files path1 path2 f =
  with_input_file path1 (fun ch1 ->
    with_input_file path2 (fun ch2 -> f ch1 ch2))

let run_test label show f =
  Printf.printf "\n%s\n%!" label;
  match f () with
  | value -> Printf.printf "result: %s\n%!" (show value)
  | exception e -> Printf.printf "exception: %s\n%!" (Printexc.to_string e)

let () =
  let data = if Array.length Sys.argv > 1 then Sys.argv.(1) else "../data" in
  let file name = Filename.concat data name in
  let count = string_of_int in

  run_test "T1 sample.txt, search red" count (fun () ->
    count_matches_ocaml (file "sample.txt") "red" ());
  run_test "T2 sample.txt, search orange" count (fun () ->
    count_matches_ocaml (file "sample.txt") "orange" ());
  run_test "T3 empty.txt, search red" count (fun () ->
    count_matches_ocaml (file "empty.txt") "red" ());
  run_test "T4 missing.txt, search red" count (fun () ->
    count_matches_ocaml (file "missing.txt") "red" ());
  run_test "T5 sample.txt, search red, forced failure" count (fun () ->
    count_matches_ocaml (file "sample.txt") "red" ~fail_after_first_line:true ());
  run_test "T6 sample.txt, empty search string" count (fun () ->
    count_matches_ocaml (file "sample.txt") "" ());
  run_test "T7 long_line.txt, search red" count (fun () ->
    count_matches_ocaml (file "long_line.txt") "red" ());

  let first_line_of_first ch1 _ = input_line ch1 in
  run_test "D1 two files, both open" Fun.id (fun () ->
    with_two_input_files (file "sample.txt") (file "long_line.txt") first_line_of_first);
  run_test "D2 two files, second file missing" Fun.id (fun () ->
    with_two_input_files (file "sample.txt") (file "missing.txt") first_line_of_first)
