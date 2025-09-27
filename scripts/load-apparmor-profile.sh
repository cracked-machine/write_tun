#!/bin/bash

# Script to load the custom AppArmor profile for Docker containers with network namespace support
# You need to modify devcontainer.json to use this profile:
# "runArgs": ["--security-opt", "apparmor=docker-netns"]

PROFILE_PATH="/workspaces/write_tun/.devcontainer/apparmor-profile"
PROFILE_NAME="docker-netns"

echo "Loading custom AppArmor profile: $PROFILE_NAME"

# Copy the profile to the AppArmor profiles directory
sudo cp "$PROFILE_PATH" "/etc/apparmor.d/$PROFILE_NAME"

# Load the profile
sudo apparmor_parser -r "/etc/apparmor.d/$PROFILE_NAME"

# Check if the profile is loaded
if aa-status | grep -q "$PROFILE_NAME"; then
    echo "✅ Profile '$PROFILE_NAME' loaded successfully"
else
    echo "❌ Failed to load profile '$PROFILE_NAME'"
    exit 1
fi

echo "You can now rebuild your devcontainer to use this profile"
