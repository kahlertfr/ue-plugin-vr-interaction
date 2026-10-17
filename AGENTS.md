# Agent instructions

These instructions apply to all work in this repository.

## Git workflow

- Perform all repository work in a dedicated Git worktree. Keep the primary checkout for reference and worktree management; do not edit, build, or commit there.
- Before making changes, inspect the current worktree, branch, and working-tree status. Preserve existing user changes.
- Start each new feature on a new branch in its own worktree. Use a descriptive `codex/<feature-name>` branch name unless the user specifies another name. Reuse that feature's worktree and branch when continuing the same feature.
- Create the worktree from the appropriate base branch or revision. Do not assume uncommitted changes in another checkout are included.
- Keep commits focused. Do not merge, push, or delete branches or worktrees unless the task authorizes it.
- When instructed to push a new branch, always use SSH transport (for example, `git@host:owner/repository.git` or `ssh://...`). Check the push URL before pushing; use an SSH push URL if the remote is configured for HTTPS.

- Do not commit scripts or helper code created to generate, rewrite, or patch project code or Unreal assets (including one-off migration commandlets). You may create a `codex/` folder at the repository root to store all such scripts, helper code, and commandlets locally. This folder must remain ignored by Git; never force-add its contents. Commit only the intended project changes these helpers produce.

## Commit attribution

- Every AI-assisted commit message must include a Git `Co-authored-by` trailer identifying the actual AI model used for the committed work.
- Use the exact model name or identifier available in the session; do not invent a model identity. If it is unavailable, obtain it before committing.
- Use this format, separated from the commit body by a blank line:

  ```text
  Co-authored-by: <actual AI model name> <model-email>
  ```

- Use a model-specific attribution email supplied by the environment or user. If none is supplied, use `<model-id>@users.noreply.github.com`, substituting a lowercase, email-safe model identifier.
- If multiple AI models contributed to the committed changes, include one trailer per contributing model. Preserve human coauthor trailers and the configured Git author.

## Project context

- This repository contains VRInteraction, an Unreal Engine 5.6 runtime plugin for VR interactions and in-scene logging. It is used by TrajectoryBLab but is a separate Git repository.
- Paths in these instructions are relative to this plugin repository. Read `README.md` before changing plugin behavior or its public logging API.
- C++ lives in `Source/VRInteraction`, with interfaces in `Public` and implementations in `Private`. Interaction Blueprints and widgets live in `Content/VR`; inspect the relevant assets as well as C++ when changing interactions.
- Inspect `VRInteraction.uplugin` and `Source/VRInteraction/VRInteraction.Build.cs` for platform support and dependencies. The plugin enables OpenXR and uses Unreal input and head-mounted-display modules.
- Preserve the GameInstance subsystem lifecycle and Blueprint-facing logging contracts. Consuming C++ modules must declare their VRInteraction dependency as described in the README.
- When used as a submodule, coordinate integration with the consuming project's worktree; record the intended plugin commit there when the task includes that integration.
- Unreal content may use Git LFS. Hydrate required assets before inspecting or editing them, and preserve LFS tracking. Do not edit binary Unreal assets as text.
- Do not commit generated build, editor, or cache output, including `Binaries`, `Intermediate`, `Saved`, and `DerivedDataCache`.

## Validation

- Build the consuming Unreal project's Editor target with this plugin enabled when the engine and dependencies are available; this plugin does not contain a standalone project target.
- For logging changes, verify subsystem message collection and the affected in-scene display Blueprint in the consuming Unreal project. For input or interaction changes, verify the relevant Blueprint behavior in the editor and use an OpenXR headset when the change requires device validation.
- Report what was checked and any checks that could not be run. Do not claim a successful build, editor session, or headset test without performing it.
