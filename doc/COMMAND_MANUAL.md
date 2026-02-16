# EasyMIPS Command Manual

This manual describes the syntax for the debugger commands: `#show`, `#set`, `#exec`, `#reset`, and `#stop`.

## Table of Contents
- [#show](#show)
- [#set](#set)
- [#exec](#exec)
- [#reset](#reset)
- [#stop](#stop)

---

## #show

Display the value of registers, memory locations, or constants.

### Syntax

```
#show <argument> [<format>] [sep = <separator>]
```

### Arguments

The `<argument>` can be:

1. **Register** - A register name (e.g., `$t0`, `$a0`, `$sp`, `$pc`)
   ```
   #show $t0
   #show $pc
   ```

2. **Memory Reference** - A memory location with optional size specifier
   ```
   #show 0x1000($t0)           # Base+offset addressing
   #show ($sp)                 # Register indirect
   #show byte 0x1000($t0)      # Byte-sized memory access
   #show hword 0x1000($t0)     # Halfword-sized memory access
   #show word 0x1000($t0)      # Word-sized memory access
   ```

3. **Constant** - A numeric or symbolic constant
   ```
   #show 42
   #show 0xFF
   #show 0b1010
   #show 'A'
   #show my_label
   ```

4. **Array/List** - Multiple values enclosed in brackets
   ```
   #show [$t0, $t1, $t2]
   #show [0x1000, 0x1004, 0x1008]
   ```

5. **Memory Range** - Base address with repeat count (using colon)
   ```
   #show (0x1000):10             # Show 10 words starting at 0x1000
   #show byte (0x1000):16        # Show 16 bytes starting at 0x1000
   ```

6. **String Literal** - For separator argument only
   ```
   #show [$t0, $t1] sep = ", "
   ```

### Format Options

The optional `<format>` specifies how to display the value:

| Format | Description |
|--------|-------------|
| `hex` or `hexadecimal` | Display in hexadecimal |
| `dec` or `decimal` | Display in decimal |
| `signed dec` or `signed decimal` | Display as signed decimal |
| `unsigned dec` or `unsigned decimal` | Display as unsigned decimal |
| `binary` | Display in binary |
| `ascii` | Display as ASCII character |

### Separator

The optional `sep = <separator>` specifies a separator string between multiple values. The separator can be a string literal:

```
#show [$t0, $t1, $t2] sep = ", "
#show [(0x1000):10] sep = "\n"
```

### Examples

```
#show $t0                          # Show register $t0 in hex
#show $t0 dec                      # Show register $t0 in decimal
#show $t0 signed dec               # Show register $t0 as signed decimal
#show 0x1000($sp)                  # Show memory at address $sp + 0x1000
#show byte 0x10($t0) binary        # Show byte at $t0 + 0x10 in binary
#show [$t0, $t1, $t2, $t3]         # Show multiple registers
#show [$t0, $t1, $t2] sep = " | "  # Show with custom separator
#show (0x1000):16                     # Show 16 words starting at 0x1000
#show word (0x2000):8 ascii           # Show 8 words as ASCII
```

---

## #set

Set the value of a register or memory location.

### Syntax

```
#set <destination> = <source>
```

### Destination

The `<destination>` can be:

1. **Register** - A register name
   ```
   #set $t0 = 42
   #set $sp = $sp - 4
   ```

2. **Memory Reference** - A memory location with optional size specifier
   ```
   #set 0x1000($t0) = 0xFF
   #set byte 0x10($sp) = 'A'
   #set hword 0x20($t1) = 1000
   #set word ($sp) = $t0
   ```

3. **Array/List** - Multiple destinations enclosed in brackets
   ```
   #set [$t0, $t1, $t2] = [1, 2, 3]
   ```

### Source

The `<source>` can be:

1. **Constant** - A numeric or character constant
   ```
   #set $t0 = 42
   #set $t0 = 0xFF
   #set $t0 = 0b1010
   #set $t0 = 'A'
   ```

2. **Register** - Copy from another register
   ```
   #set $t0 = $t1
   ```

3. **Memory Reference** - Read from memory
   ```
   #set $t0 = 0x1000($sp)
   #set $t0 = word ($t1)
   ```

4. **Expression** - Using high/low word operators
   ```
   #set $t0 = #hihw(0x12345678)    # Extract high 16 bits
   #set $t0 = #lohw(0x12345678)    # Extract low 16 bits
   ```

5. **Array/List** - Multiple values enclosed in brackets
   ```
   #set [$t0, $t1, $t2] = [1, 2, 3]
   #set [0x1000, 0x1004] = [0xFF, 0xAA]
   ```

### Examples

```
#set $t0 = 42                          # Set register $t0 to 42
#set $t0 = 0x1000                      # Set register $t0 to 0x1000
#set $t0 = $t1                         # Copy $t1 to $t0
#set 0x1000($sp) = 0xFF                # Store 0xFF at memory address $sp + 0x1000
#set byte 0x10($t0) = 'A'              # Store ASCII 'A' as byte at $t0 + 0x10
#set hword 0x20($t1) = 1000            # Store 1000 as halfword at $t1 + 0x20
#set word ($sp) = $t0                  # Store $t0 at address in $sp
#set [$t0, $t1, $t2] = [1, 2, 3]      # Set multiple registers at once
#set $t0 = #hihw(0x12345678)           # Set $t0 to 0x1234 (high word)
#set $t0 = #lohw(0x12345678)           # Set $t0 to 0x5678 (low word)
```

---

## #exec

Execute a system command or script.

### Syntax

```
#exec "<command>"
```

### Description

The `#exec` command executes an external command or script specified as a string literal. The command is passed to the system shell for execution.

### Examples

```
#exec "ls -la"
#exec "cat output.txt"
#exec "./run_test.sh"
#exec "gcc -o program program.c"
```

---

## #reset

Reset the virtual machine state.

### Syntax

```
#reset
```

### Description

The `#reset` command resets the MIPS32 virtual machine to its initial state. This typically includes:
- Resetting all registers to their default values
- Clearing memory
- Resetting the program counter to the starting address

### Examples

```
#reset
```

---

## #stop

Stop the execution of the program.

### Syntax

```
#stop
```

### Description

The `#stop` command halts the execution of the currently running program in the MIPS32 virtual machine. This can be used to terminate program execution, typically during debugging or when a breakpoint condition is met.

### Examples

```
#stop
```

---

## Common Argument Types

### Constants

| Type | Syntax | Example |
|------|--------|---------|
| Decimal | `123` | `42` |
| Hexadecimal | `0x123` | `0xFF` |
| Binary | `0b1010` | `0b11110000` |
| Character | `'A'` | `'\n'` |
| Identifier | `label_name` | `my_label` |

### Registers

Registers can be specified by name:
- General purpose: `$zero`, `$at`, `$v0-$v1`, `$a0-$a3`, `$t0-$t9`, `$s0-$s7`, `$k0-$k1`
- Special: `$gp`, `$sp`, `$fp`, `$ra`, `$pc`
- HI/LO: `$hi`, `$lo`

Register indices can also be used: `$0`, `$1`, `$2`, ..., `$31`

### Memory Addressing

| Mode | Syntax | Description |
|------|--------|-------------|
| Register indirect | `($reg)` | Address is in register |
| Base + offset | `offset($reg)` | Address is register + offset |
| Byte access | `byte offset($reg)` | Access 1 byte |
| Halfword access | `hword offset($reg)` | Access 2 bytes |
| Word access | `word offset($reg)` | Access 4 bytes |

### Size Specifiers

| Specifier | Size | Bytes |
|-----------|------|-------|
| `byte` | 8 bits | 1 |
| `hword` | 16 bits | 2 |
| `word` | 32 bits | 4 |

### Special Operators

| Operator | Syntax | Description |
|----------|--------|-------------|
| High word | `#hihw(expr)` | Extract upper 16 bits of expression |
| Low word | `#lohw(expr)` | Extract lower 16 bits of expression |

---

## Notes

- All commands start with `#` prefix
- All commands must be on a single line
- Whitespace between tokens is flexible
- Comments starting with `;` are ignored
- String literals for `#exec` and separators must be enclosed in double quotes
- The separator in `#show` can be any string literal
- The lexer enters special states for `#show` and `#set` commands to recognize format keywords
