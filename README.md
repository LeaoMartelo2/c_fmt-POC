# Proof of concept of string formatting in C with braces `{}` and positional arguments;

# This is extremelly slow, do not use in serious projects
it might not look like it, but its extremelly slow and prone to breaking, and only a proof of concept.
It might not look that slow because printing to a terminal is slower than it :trollface:

```c 
fmt("Config data: {{ \"name\": \"{0}\", \"version\": {1}, \"radius\": {2:.3} }}\n",
            fmt_arg("re"),
            fmt_arg(2),
            fmt_arg(123.321321321312));

fmt("this {} so {}, king\n",
            fmt_arg("is"),
            fmt_arg(true));
```

format with `{}`, or specify argument position with `{number}`, the next argument, if not numbered will be the next sequential integer.

use pŕesision specifiers with `:`
such as:

`{2:.3}`
will format second argument up to 3 decimal places
