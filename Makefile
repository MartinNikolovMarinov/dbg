CC       = g++
CFLAGS   = -ggdb -std=c++17 -Wno-unknown-pragmas -O0 -g -Wall -Wno-unused
SRC      = $(wildcard *.cpp)
OUT_DIR  = build
BIN_NAME = main

all: build

.PHONY: help
help:
	@echo Supported targets:
	@cat $(MAKEFILE_LIST) | grep -e "^[\.a-zA-Z0-9_-]*: *.*## *" | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-35s\033[0m %s\n", $$1, $$2}'

.PHONY: build
build: ## Build the project in the build folder. Creates ./build folder if it does not exist.
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -o $(OUT_DIR)/$(BIN_NAME) $(SRC)
