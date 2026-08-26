type nat = 
| Z (* Represents zero *)
| S of nat (* Represents the successor of a natural number *)

let rec to_int : nat -> int = function
  | Z -> 0
  | S n -> 1 + to_int n

  