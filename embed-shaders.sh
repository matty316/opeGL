#!/bin/bash

VERT_SOURCE="shaders/shader.vert"
MODEL_VERT_SOURCE="shaders/model.vert"
FRAG_SOURCE="shaders/shader.frag"
VERT_CPP_HEADER="include/vert-shader.h"
MODEL_CPP_HEADER="include/model-shader.h"
FRAG_CPP_HEADER="include/frag-shader.h"

xxd -i "$VERT_SOURCE" >"$VERT_CPP_HEADER"
xxd -i "$MODEL_VERT_SOURCE" >"$MODEL_CPP_HEADER"
xxd -i "$FRAG_SOURCE" >"$FRAG_CPP_HEADER"
