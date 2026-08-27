type nat = 
| Z (* Represents zero *)
| S of nat (* Represents the successor of a natural number *)

(* Converts Peano numbers to standard integers *)
let rec to_int : nat -> int = function
  | Z -> 0
  | S n -> 1 + to_int n

(* Adds two Peano numbers x and y *)  
let rec add : nat -> nat -> nat = fun x y ->
  match x with
  | Z -> y                                    (* Base case: 0 + y is just y *)
  | S x_prev -> S (add x_prev y)              (* Strips outer S from x, adds remaining x_prev to y, wraps result in S *)

(* Peano Multiplication *)
let rec mult : nat -> nat -> nat = fun x y ->
  match x with
  | Z -> Z                                    (* Base case: 0 * y is Z (0) *)
  | S x_prev -> add y (mult x_prev y)         (* Strips outer S from x, recursively multiplies remaining x_prev by y, then adds one copy of y *)

(* Peano Subtraction *)
let rec sub : nat -> nat -> nat = fun x y ->
    match x, y with
    | x, Z -> x                               (* Base case: x - 0 is x *)
    | S x_prev, S y_prev -> sub x_prev y_prev (* Strips outer S from both x and y, recursively subtracts remaining values *)
    | Z, S _ -> Z (* If y is greater than x, return Z *)

(* Peano Comparison *)
let rec is_less : nat -> nat -> bool = fun x y ->
  match x, y with
  | Z, S _ -> true                            (* If x runs out of S shells first while y still has S shells, x is less than y *)
  | Z, Z -> false                             (* If both hit Z at the exact same time, x is equal to y, so it is NOT less than y *)
  | S _, Z -> false                           (* If y runs out of S shells first while x still has S shells, x is greater than y *)
  | S x_prev, S y_prev -> is_less x_prev y_prev (* Strips one S shell off both numbers and recursively compares remaining values *)

(* Peano Division *)
let rec div : nat -> nat -> nat = fun dividend divisor ->
  match divisor with
  | Z -> invalid_arg "division by zero :("
  | S _ ->
      if is_less dividend divisor then Z      (* Base case: If dividend is smaller than divisor, dividend / divisor is 0 *)
      else S (div (sub dividend divisor) divisor) (* Subtracts divisor from dividend, recursively divides remainder, and counts step with S *)

(* Test *)
let () =
  let zero = Z in
  let one = S Z in
  let two = S (S Z) in
  let three = S (S (S Z)) in
  let four = S (S (S (S Z))) in
  let five = S (S (S (S (S Z)))) in
  let six = S (S (S (S (S (S Z))))) in

  print_endline "Peano Arithmetic Tests :D";

  (* Multiplication *)
  print_string ("2 * 3 = ");
  print_int (to_int (mult two three)); (* Expected: 6/S (S (S (S (S (S Z))))) *)
  print_newline ();

  (* Subtraction *)
  print_string ("4 - 2 = ");
  print_int (to_int (sub four two)); (* Expected: 2/S (S Z) *)
  print_newline ();

  (* Comparison *)
  print_string ("Is 2 < 3? ");
  print_string (string_of_bool (is_less two three)); (* Expected: true *)
  print_newline ();

  (* Division *)
  print_string ("6 / 2 = ");
  print_int (to_int (div six two)); (* Expected: 3/S (S (S Z)) *)
  print_newline ();

  (* More Division *)
  print_string ("5 / 1 = ");
  print_int (to_int (div five one)); (* Expected: 5/S (S (S (S (S Z)))) *)
  print_newline ();

(* Division by zero test *)
print_string ("4 / 0 = ");
try 
    print_int (to_int (div four zero)) (* Expected: Error *)
    with Invalid_argument msg ->
    print_string ("Error: " ^ msg);
  print_newline ();
