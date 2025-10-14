#!/bin/bash

VERT_SOURCE="shaders/shader.vert"
FRAG_SOURCE="shaders/shader.frag"
VERT_CPP_HEADER="include/vert-shader.h"
FRAG_CPP_HEADER="include/frag-shader.h"

xxd -i "$VERT_SOURCE" >"$VERT_CPP_HEADER"
xxd -i "$FRAG_SOURCE" >"$FRAG_CPP_HEADER"
