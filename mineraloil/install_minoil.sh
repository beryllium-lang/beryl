#!/bin/bash

SCRIPT_NAME="__main__.py"
# Use /usr/local/bin for user-installed binaries on macOS
INSTALL_DIR="/usr/local/bin"
COMMAND_NAME="mineraloil"
ALIAS_NAME="minoil"

# Create the directory if it doesn't exist (may require sudo)
if [ ! -d "$INSTALL_DIR" ]; then
    sudo mkdir -p "$INSTALL_DIR"
fi

# Copy and set permissions
echo "Installing $COMMAND_NAME to $INSTALL_DIR..."
sudo cp "$SCRIPT_NAME" "$INSTALL_DIR/$COMMAND_NAME"
sudo chmod +x "$INSTALL_DIR/$COMMAND_NAME"

# Create the symbolic link for the alias
sudo ln -sf "$INSTALL_DIR/$COMMAND_NAME" "$INSTALL_DIR/$ALIAS_NAME"

# Update PATH in .zshrc (default for macOS)
if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
    echo "Adding $INSTALL_DIR to PATH in .zshrc..."
    echo "export PATH=\"$INSTALL_DIR:\$PATH\"" >> "$HOME/.zshrc"
    echo "Please run 'source ~/.zshrc' or restart your terminal."
fi

echo "------------------------------------------"
echo "MineralOil installed or updated!"
echo "You can now run: $COMMAND_NAME or $ALIAS_NAME"