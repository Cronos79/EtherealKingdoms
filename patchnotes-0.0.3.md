# Ethereal Kingdoms - Patch Notes `0.0.3.0`

> **Build Date**: 2025-05-14  
> **Stage**: Pre-Alpha – Kingdom Simulation Core

---

## ✨ Major Features

- 🏰 **Buildings System**
  - Added support for kingdom buildings, including the Town Center and Wheat Farm.
  - Buildings are saved/loaded per kingdom and persist across sessions.
  - Each building tracks its own state, level, workers, and construction progress.
  - Buildings can apply one-time or passive effects (e.g., Town Center increases population cap once, produces coins daily).

- 📊 **Kingdom Stats**
  - Introduced a dedicated stats system for kingdoms (Population, Population Cap, Happiness, Military Strength).
  - Stats are stored and loaded separately from resources.
  - Stats are updated by building effects and kingdom events.

---

## 🛠 Improvements

- Refactored database schema to separate resources and stats for scalability and clarity.
- Building effects (such as population cap boosts) are now persistent and only applied once per building, even across server restarts.
- Improved save/load logic for buildings, including new fields for one-time effect tracking.

---

## 🧪 Notes

- All previous resource and kingdom creation features remain.
- This update lays the foundation for more complex simulation and management mechanics in future builds.