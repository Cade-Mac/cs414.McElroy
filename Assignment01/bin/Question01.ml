type nat = 
| Z (* Represents zero *)
| S of nat (* Represents the successor of a natural number *)

(* Converts Peano numbers to standard integers *)
let rec to_int : nat -> int = function
  | Z -> 0
  | S n -> 1 + to_int n
  
let rec add : nat -> nat -> nat = fun x y ->
  match x with 
  | Z -> y
  | S n -> S (add n y)

  (* Peano Multiplication *)
let rec mult : nat -> nat -> nat = fun x y ->
  match x with
  | Z -> Z
  | S n -> add y (mult n y)

  (* Peano Subtraction*)
let rec sub : nat -> nat -> nat = fun x y ->
 match x, y with
 | x, Z -> x
 | S x_prev, S y_prev -> sub x_prev y_prev
 | Z, S _ -> Z  (* If y is greater than x, return Z *)

  (* Peano Comparison *)
let rec is_less : nat -> nat -> bool = fun x y ->
  match x, y with
  | Z, S _ -> true
  | Z, Z -> false
  | S _, Z -> false
  | S x_prev, S y_prev -> is_less x_prev y_prev

  (* Peano Division *)
let rec div : nat -> nat -> nat = fun dividend divisor ->
  match divisor with
  | Z -> invalid_arg "Division by zero :("
  | S _ ->
    if is_less dividend divisor then Z
    else S (div (sub dividend divisor) divisor)

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
    with Invalid_argument msg->
    print_string ("Error: " ^ msg);
  print_newline ();  