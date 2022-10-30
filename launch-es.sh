#!/bin/bash
# Created by Sam Gleske
# DESCRIPTION
#     This is a launcher script designed to run from Linux.  It checks for
#     updates to Sam Gleske's fork with plugins and if an update is required,
#     then automatically fetches the latest version.  This is useful for steam
#     deck.
#
# INSTALL:
#
#     1. Switch to steam deck desktop mode.
#     2. Open konsole terminal.
#     3. Run the following commands.
#
#         curl -Lo ~/launch-es.sh https://raw.githubusercontent.com/samrocketman/endless-sky/mining-complete-plugins/launch-es.sh
#         chmod 755 ~/launch-es.sh
#
#     4. Switch back to steam deck by clicking shut down > log out.
#     5. Go to Endless Sky and select properties.  Change the launch options to
#        one of the commands listed under USAGE.
#
# USAGE:
#     Launch the game normally from steam launch options.
#
#         ~/launch-es.sh %command%
#
#     Force checking for updates every time.
#
#         ~/launch-es.sh forceupdate %command%
#
#     Never check for updates.  Warning: you may miss out on critical bug fixes
#     and new content.
#
#         ~/launch-es.sh skipupdate %command%
#
# CONTROLS:
#
#     1. Install the "samrocketman endless sky steam deck" controls.
#     2. While in space, pressing R5 button will toggle mouse control.

if [ -n "${DEBUG:-}" ]; then
  set -exo pipefail
else
  set -eo pipefail
fi

APPIMAGE_URL="https://github.com/samrocketman/endless-sky/releases/download/continuous-with-plugins/endless-sky-x86_64-continuous-with-plugins.AppImage"
CHECKSUM_URL="${APPIMAGE_URL}.sha256sum"

# persist timestamp for successful update check
function save_last_update_timestamp() {
  date +%s > ~/.cache/endless-sky/lastUpdate
}

# returns true if an update check is required.  Returns false if not elapsed.
function should_check_for_update() {
  if [ ! -f ~/.cache/endless-sky/lastUpdate ]; then
    return 0
  fi
  if [ ! -f ~/.cache/endless-sky/"${APPIMAGE_URL##*/}" ]; then
    return 0
  fi
  local last_update="$(< ~/.cache/endless-sky/lastUpdate)"
  local now_timestamp="$(date +%s)"
  # 3600*12 is 12 hrs
  local twelve_hours=43200
  [ "$(( now_timestamp-last_update ))" -gt "${twelve_hours}" ]
}

# Compare currently downloaded game with a remote checksum and download update
# if mismatched.
function update_game() (
  # unset steam runtime variable
  unset LD_LIBRARY_PATH

  if ! should_check_for_update; then
    return 0
  fi
  if ! timeout 5 curl -sSfIo /dev/null https://github.com/; then
    # no internet so continue with playing the game
    return 0
  fi
  echo 'Check for updates' >&2
  if ! curl -sSfLO "${CHECKSUM_URL}"; then
    return 1
  fi
  cd ~/.cache/endless-sky
  if sha256sum -c - < "${TMP_DIR%/}/${CHECKSUM_URL##*/}"; then
    save_last_update_timestamp
    return 0
  fi
  echo 'Downloading new update' >&2
  cd "${TMP_DIR}"
  if ! curl -sSfLO "${APPIMAGE_URL}"; then
    return 1
  fi
  if sha256sum -c - < "${CHECKSUM_URL##*/}"; then
    chmod 755 "${APPIMAGE_URL##*/}"
    mv -f "${APPIMAGE_URL##*/}" ~/.cache/endless-sky/
    save_last_update_timestamp
    return 0
  else
    return 1
  fi
)

function set_preference() (
  local prefs=~/.local/share/endless-sky/preferences.txt
  [ -d ~/.local/share/endless-sky ] || mkdir -p ~/.local/share/endless-sky
  [ -f "$prefs" ] || touch "$prefs"
  if ! grep "$1" "$prefs" &> /dev/null; then
    echo "$1 $2" >> "$prefs"
    return
  fi
  if ! grep "$1 $2" "$prefs" &> /dev/null; then
    sed -i "s/^${1}.*/$1 $2/" "$prefs"
  fi
)

#
# MAIN program
#

if [ ! -d ~/.cache/endless-sky ]; then
  mkdir -p ~/.cache/endless-sky
fi

# debug logs
if [ -n "${DEBUG:-}" ]; then
  exec &> ~/.cache/endless-sky/launch-es.log
fi

TMP_DIR="$(mktemp -d)"
trap 'cd;rm -rf "${TMP_DIR}"' EXIT


if [ "$1" = forceupdate ]; then
  rm -f ~/.cache/endless-sky/lastUpdate
fi

cd "${TMP_DIR}"
tries=1
until [ "$1" = skipupdate ] || update_game; do
  if [ "${tries}" -gt 3 ]; then
    break
  else
    (( tries=tries+1 ))
  fi
  sleep 3
done
EXE_FILE=~/.cache/endless-sky/"${APPIMAGE_URL##*/}"
if [ ! -x "${EXE_FILE}" ]; then
  echo 'No executable found,  Check your internet connection and try again.' 2>&1
  exit 1
else
  #set_preference fullscreen 1
  #set_preference maximized 1
  exec "${EXE_FILE}"
fi
