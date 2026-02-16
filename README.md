# Smol

A small compiler for a toy language.

## Building and running

To build the following program, you can run the following command:

```bash
make
```

This should create a compiler in `./build` directory

## Testing

After building you can run the test program

```bash
./build/smol tests/fib.smol
```

This should run the fibonacci program without any problem.

You can also run code from command line.

```
echo "var a = 12; print a * 2;" | ./build/smol -
```

## More info

For more info regarding the usage run the following

```bash
./build/smol --help
```

## Resources

- [Smol Language Grammar](./grammar)
