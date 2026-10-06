open Store

let rec loop (current_store : store) : unit =
  print_string "> ";
  flush stdout;
  try
    let line = read_line () in
    let parts = String.split_on_char ' ' line |> List.filter (fun s -> s <> "") in
    match parts with
    | ["QUIT"] | ["quit"] -> ()
    | ["SET"; k; v] -> loop (set k v current_store)
    | ["GET"; k] ->
        (match get k current_store with
         | Some v -> print_endline v
         | None -> print_endline "(not found)");
        loop current_store
    | ["DELETE"; k] -> loop (delete k current_store)
    | ["LIST"] ->
        List.iter (fun (k, v) -> Printf.printf "%s = %s\n" k v) (list current_store);
        loop current_store
    | ["SAVE"; filename] ->
        save filename current_store;
        loop current_store
    | ["LOAD"; filename] -> loop (load filename)
    | _ -> loop current_store
  with End_of_file -> ()

let () = loop empty