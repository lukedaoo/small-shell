#!/bin/sh
# gather.sh
# Gathers source files and generates a minimal Makefile in a target directory.

if [ -z "$1" ]; then
    echo "Usage: $0 <target_directory>"
    exit 1
fi

TARGET_DIR="$1"
COMPILER="gcc"
SOURCE_FILE="mysh.c"
EXECUTABLE="mysh"
MAKE_FILE="Makefile"

mkdir -p "$TARGET_DIR"

if [ -f "$SOURCE_FILE" ]; then
    echo "Copying $SOURCE_FILE to $TARGET_DIR"
    cp "$SOURCE_FILE" "$TARGET_DIR"
else
    echo "Error: $SOURCE_FILE not found."
    exit 1
fi

echo "Generating $MAKE_FILE in $TARGET_DIR"
cat <<EOF > "$TARGET_DIR/$MAKE_FILE"
run:
	$COMPILER $SOURCE_FILE -o $EXECUTABLE && ./$EXECUTABLE

clean:
	rm -f $EXECUTABLE
EOF

echo "Files gathered and Makefile created in: $TARGET_DIR"
