#!/bin/bash

MK42_SRC="mk42.c"
MK42_BIN="$HOME/bin/mk42"
SHELL_RC=""
ZSHENV="$HOME/.zshenv"

# Detect shell and config file
if [[ "$SHELL" == *"bash" ]]; then
    SHELL_RC="$HOME/.bashrc"
elif [[ "$SHELL" == *"zsh" ]]; then
    SHELL_RC="$HOME/.zshrc"
else
    echo "Unsupported shell. Please add $HOME/bin to your PATH manually."
    exit 1
fi

# Compile mk42
if [ ! -f "$MK42_SRC" ]; then
    echo "Source file $MK42_SRC not found in current directory."
    exit 1
fi

echo "Compiling mk42..."
gcc "$MK42_SRC" -o mk42
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

mkdir -p "$HOME/bin"
mv mk42 "$MK42_BIN"
echo "Moved mk42 executable to $MK42_BIN"

add_path_if_missing() {
    local file="$1"
    if [ -f "$file" ]; then
        if ! grep -q 'export PATH="$HOME/bin:$PATH"' "$file"; then
            echo "" >> "$file"
            echo "# Added by mk42 installer" >> "$file"
            echo 'export PATH="$HOME/bin:$PATH"' >> "$file"
            echo "Added ~/bin to PATH in $file"
        else
            echo "~/bin is already in PATH in $file"
        fi
    else
        echo "File $file does not exist, creating it..."
        echo '#!/bin/sh' > "$file"
        echo 'export PATH="$HOME/bin:$PATH"' >> "$file"
        echo "Created $file and added ~/bin to PATH"
    fi
}

add_path_if_missing "$SHELL_RC"

# If using zsh, check if oh-my-zsh is installed (by existence of ~/.oh-my-zsh)
if [[ "$SHELL" == *"zsh" ]] && [ -d "$HOME/.oh-my-zsh" ]; then
    add_path_if_missing "$ZSHENV"
fi

# Reload shell config
echo "Reloading shell config..."
source "$SHELL_RC"

echo "Installation complete! You can now use 'mk42 <project_name>' from any directory."
