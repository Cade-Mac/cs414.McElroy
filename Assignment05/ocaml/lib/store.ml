module StringMap = Map.Make(String)

type store = string StringMap.t

let empty : store = StringMap.empty

let set (k : string) (v : string) (s : store) : store =
  StringMap.add k v s

let get (k : string) (s : store) : string option =
  StringMap.find_opt k s

let delete (k : string) (s : store) : store =
  StringMap.remove k s

let list (s : store) : (string * string) list =
  StringMap.bindings s

let save (filename : string) (s : store) : unit =
  let oc = open_out filename in
  StringMap.iter (fun k v -> Printf.fprintf oc "%s %s\n" k v) s;
  close_out oc

let load (filename : string) : store =
  try
    let ic = open_in filename in
    let rec read_lines acc =
      try
        let line = input_line ic in
        match String.split_on_char ' ' line with
        | [k; v] -> read_lines (StringMap.add k v acc)
        | _ -> read_lines acc
      with End_of_file ->
        close_in ic;
        acc
    in
    read_lines StringMap.empty
  with _ -> StringMap.empty

let run_transaction (f : store -> (store, string) result) (s : store) : store =
  match f s with
  | Ok new_store -> new_store
  | Error _ -> s (* Rollback: returns untouched original store *)