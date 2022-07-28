# nanopb_printer
Print nanopb data in nice way, almost same as Python implementation


# Example

nanopb_printer:

```
╰─ ./simple.out                                                                           ─╯
number: 99

lucky_number: 42
unlucky_number: 0
enum1: TWO

lucky_number: 23
enum1: TWOTWO
byte: "a55a"


nested2 {
}
enum1: ONE
enum2: TWOTWO

text: "asdf"
text: "zxcv"
message {
        lucky_number: 0
        unlucky_number: 169
        enum1: ONE
}
message {
        lucky_number: 0
        unlucky_number: 144
        enum1: ONE
}

unlucky_number: 123
notnested: 124

msg1 {
        lucky_number: 42
        unlucky_number: 24
        enum1: TWO
}
notnested: 142

included {
        enum_inc: EXCLUDED
}
```

Python:

```
╰─ python3 example.py                                                                    ─╯
number: 99

lucky_number: 42
unlucky_number: 0
enum1: TWO

lucky_number: 23
enum1: TWOTWO
byte: "\245Z"

nested2 {
}
enum1: ONE
enum2: TWOTWO

text: "asdf"
text: "zxcv"
message {
  lucky_number: 0
  unlucky_number: 169
  enum1: ONE
}
message {
  lucky_number: 0
  unlucky_number: 144
  enum1: ONE
}

unlucky_number: 123
notnested: 124

msg1 {
  lucky_number: 42
  unlucky_number: 24
  enum1: TWO
}
notnested: 142

included {
  enum_inc: EXCLUDED
}

```

Differences:
1. Indentetion: spaces vs tabs
2. Binary formats: hex (`a55a`) vs Python (`\245Z`)

# Usage

1. `make clean`
1. `make proto`
1. `make parser`
1. `make example && ./example/example`
1. `make tests && ./tests/tests`
