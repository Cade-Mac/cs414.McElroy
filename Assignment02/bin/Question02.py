import re
# C++ Identifiers: Starts with letter/underscore, followed by letters, digits, or underscores
cpp_identifier_pattern = r"^[a-zA-Z_][a-zA-Z0-9_]*$"

# Phone Numbers: Matches formats like (123) 456-7890 or 123-456-7890
phone_number_pattern = r"^(\(\d{3}\)\s?|\d{3}-)\d{3}-\d{4}$"

# Floating Point Numbers: Matches numbers like 123.45, -0.67, +3.14e-10
floating_number_pattern = r"^[+-]?(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?$"

# Binary Palindromes of Length 3 or 4:
# Length 3: 000, 010, 101, 111  -> Pattern: ^(0[01]0|1[01]1)$
# Length 4: 0000, 0110, 1001, 1111 -> Pattern: ^(0[01]{2}0|1[01]{2}1)$
binary_pal_pattern = r"^(0[01]0|1[01]1|0[01]{2}0|1[01]{2}1)$"

def test_regex(name, pattern, test_cases):
    print(f"=== Testing: {name} ===")
    for t in test_cases:
        matched = bool(re.match(pattern, t))
        print(f"  {t:<18} -> {matched}")
    print()

# Test cases for each regex pattern

if __name__ == "__main__":
    test_regex("C++ Identifiers", cpp_identifier_pattern, ["_myVar1", "counter", "2fast", "class-name"])
    test_regex("US Phone Numbers", phone_number_pattern, ["(123) 456-7890", "123-456-7890", "(123)456-7890", "1234567890"])
    test_regex("Floating Point Numbers", floating_number_pattern, ["-3.1415", "+0.005", "42", ".75", "abc"])
    test_regex("Binary Palindromes (Len 3 or 4)", binary_pal_pattern, ["101", "0110", "1001", "1101", "0101"])
