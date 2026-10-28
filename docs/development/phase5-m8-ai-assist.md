# Phase 5.7 — Milestone 8: AI-assisted CAD

## Goal

Translate supported natural-language requests into a structured operation plan, validate it, preview it, require confirmation, and execute only through Feature History.

## Usage

**Tools → AI Assist…**

Example prompt:

> Create an aluminum enclosure measuring 100 × 60 × 30 mm, with 3 mm walls and four mounting holes.

Also supported: `box 100 x 60 x 30`, `cylinder r=25 h=60`, `sphere r=40`.

## Safety

See [ADR 0004](../decisions/0004-ai-assist-safety.md). No arbitrary code execution; no direct memory writes from AI text.
