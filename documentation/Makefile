# Variables
PRESENTATION = Presentation.md
OUTPUT_DIR = docs

# Default target
.PHONY: all
all: main

# Generate static site from presentation
.PHONY: main
main:
	reveal-md $(PRESENTATION) --static $(OUTPUT_DIR)

# Watch presentation for changes and reload
.PHONY: test
test:
	reveal-md $(PRESENTATION) -w

# Clean generated files
.PHONY: clean
clean:
	rm -rf $(OUTPUT_DIR)
