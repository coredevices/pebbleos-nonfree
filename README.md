This repository contains files used to build the Pebble firmware that are not available under an Apache 2.0 license as the rest of the firmware code is.

## SF32LB52 LCPU patch

`sf32lb52/lcpu_patch_rev_b.c` is the patch as delivered by the vendor. The
firmware uses `sf32lb52/lcpu_patch_rev_b.h`, generated from it. When a new
patch arrives, replace the C file, regenerate the header and commit both:

```
python3 sf32lb52/gen_lcpu_patch.py sf32lb52/lcpu_patch_rev_b.c sf32lb52/lcpu_patch_rev_b.h
```

The script fails if the vendor file changes layout or install logic; review
the change before adapting the script.
