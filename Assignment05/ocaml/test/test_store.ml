open Store

let () =
  (* Test Core Operations *)
  let s0 = empty in
  let s1 = set "x" "10" s0 in
  assert (get "x" s1 = Some "10");
  let s2 = delete "x" s1 in
  assert (get "x" s2 = None);
  print_endline "[PASS] OCaml Core Operations Test";

  (* Test Transaction Abort Scenario *)
  let initial = set "x" "10" empty in
  let tx_fail s =
    let s' = set "x" "20" s in
    let _s'' = set "y" "30" s' in
    Error "Abort transaction"
  in
  let final_store = run_transaction tx_fail initial in
  assert (get "x" final_store = Some "10");
  assert (get "y" final_store = None);
  print_endline "[PASS] OCaml Transaction Abort Test";

  (* Test Save and Load *)
  let s_out = empty |> set "a" "1" |> set "b" "2" in
  save "test_ocaml.txt" s_out;
  let s_in = load "test_ocaml.txt" in
  assert (get "a" s_in = Some "1");
  assert (get "b" s_in = Some "2");
  print_endline "[PASS] OCaml Save/Load Test"