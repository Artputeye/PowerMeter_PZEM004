# Modern UI Design Skill

## Purpose
Use this skill whenever designing, redesigning, or improving a web UI.

The goal is a modern, clean, professional, premium, responsive and easy-to-use interface, especially for ESP32, IoT, energy monitoring, power meters, solar systems and hybrid inverter dashboards.

## Design Principles
- Prefer modern dashboard/application aesthetics.
- Use clean surfaces, subtle borders, controlled shadows and consistent spacing.
- Avoid excessive gradients, glossy effects, heavy shadows, excessive rounded cards and decorative elements.
- Every visual element must have a purpose.
- Improve hierarchy, usability, spacing, typography and information density.

## Visual Hierarchy
Recommended hierarchy:
1. Application header
2. Navigation
3. Page title/context
4. Primary information
5. Secondary information
6. Actions
7. Supporting information

Use size, weight, spacing and contrast to establish hierarchy. Do not rely on color alone to communicate meaning.

## Layout
- Use CSS Grid or Flexbox.
- Use a common responsive content container.
- Recommended maximum width: 1200px.
- Keep sections aligned to a common grid.
- Avoid manual positioning unless specifically required.

## Spacing
Use a consistent scale: 4, 8, 12, 16, 20, 24, 32, 40, 48px.
Use larger spacing between major sections and smaller spacing inside components.

## Typography
Prefer Inter or system-ui based sans-serif fonts.

Recommended:
- Page title: 24-32px, weight 700
- Section title: 18-22px, weight 600
- Card title: 14-16px, weight 600
- Primary value: 24-36px, weight 700
- Body: 14-16px
- Secondary text: 12-14px

Do not use too many font sizes.

## Color System
Create reusable design tokens.

Default direction:
- Page background: light neutral
- Surface: white or slightly tinted
- Text: dark neutral
- Secondary text: gray
- Border: subtle gray
- Primary: neutral dark, such as #454746
- Success: green
- Warning: amber
- Danger: red
- Info: blue

Do not make every component colorful.

## Header
The header is a major visual anchor.

Requirements:
- Clear application identity
- Consistent height
- Balanced horizontal spacing
- Strong but not oversized typography
- Optional system status
- Responsive behavior

For dark headers, a professional neutral color such as #454746 is preferred when it matches the existing project style. Avoid excessive gradients.

## Navigation
Navigation must visually belong to the application.
Use compact but comfortable height, clear active state, consistent icon/text spacing and obvious hover/focus states.
Navigation should not be unnecessarily tall.
Mobile navigation should collapse cleanly.

## Cards
Cards organize information; they should not exist merely as decoration.
Use cards for meaningful groups of information.
Do not turn every small value into a separate card.
Prefer subtle borders and shadows.

## Dashboard Metrics
For monitoring systems, prioritize important values such as Power, Voltage, Current, Energy, Battery and Grid status.
Primary metrics must be immediately visible.
Do not waste large screen areas on decoration.

## Status Indicators
Use compact status indicators such as Online, Offline, Charging, Discharging, Grid, Solar, Normal, Warning and Error.
Prefer small status dots or compact badges.
Do not use giant colored status boxes unless the state is critical.

## Buttons
Create clear hierarchy:
- Primary: Save, Apply, Start, Connect
- Secondary: Cancel, Reset, Details
- Danger: Delete, Factory Reset, Stop

Do not make every button visually equal.
Important touch targets should be approximately 44x44px or larger.

## Forms
Forms should be simple and readable.
Use clear labels, adequate spacing and logical groups.
Group related settings into sections such as Network, Power Meter, Display and System.
Avoid putting too many fields in one horizontal row.

## Tables
Prioritize readability.
Use clear headers, adequate row height, subtle separators and consistent units.
Numeric values should normally be right-aligned.
Avoid heavy borders around every cell.

## Charts
Every chart should answer a useful question.
Use clear titles, useful units, appropriate scales and minimal visual noise.
Avoid excessive grid lines, unnecessary legends and decorative charts.

## Icons
Use icons only when they improve recognition.
Keep icon size and visual weight consistent.
Prefer SVG or the project's existing icon system.
Do not use random emoji as primary UI icons.

## Responsive Design
The UI must work on desktop, laptop, tablet and mobile.

On mobile:
- Reduce horizontal padding
- Stack cards
- Collapse navigation
- Keep buttons easy to tap
- Prevent horizontal scrolling
- Maintain readable text

## Embedded / ESP32 Constraints
Performance is part of good design.

Avoid unnecessary:
- Large JavaScript frameworks
- Large image assets
- External font dependencies
- Heavy animation
- Excessive polling

Prefer lightweight HTML/CSS/JS, SVG icons, CSS variables, efficient WebSocket updates and existing project components.

A beautiful UI that consumes excessive ESP32 RAM, flash or CPU is not a successful design.

## Animation
Animation should communicate state or improve interaction.
Good uses include hover, modal appearance, status transitions, loading indicators and small chart transitions.
Avoid constant bouncing, large page animations and distracting motion.
Use short, subtle transitions.

## Dark Mode
If dark mode exists, design it intentionally rather than simply inverting colors.
Use dark gray surfaces instead of pure black everywhere.
Maintain readable contrast and clear separation between surfaces.

## Accessibility
Maintain sufficient color contrast, visible focus states, keyboard accessibility, meaningful labels and adequate touch targets.
Never rely only on color to communicate status.

## Consistency Across Pages
When modifying an existing project, inspect the current UI system first.
Reuse the existing header, navigation, colors, typography, cards, buttons, spacing and status components.
Do not create a completely different visual language for one page.
All pages must feel like the same application.

## UI Review Checklist
Before completion, verify:
- Modern appearance
- Clear visual hierarchy
- Consistent spacing and alignment
- Controlled color usage
- Purposeful cards
- Consistent typography
- Balanced header/navigation
- Obvious primary actions
- Clear status indicators
- Responsive desktop/tablet/mobile layouts
- No horizontal overflow
- Lightweight embedded implementation

## Design Decision Rule
When two designs are both functional, choose the one that is simpler, cleaner, easier to scan, more consistent, more responsive and less resource-intensive.
Do not add UI elements simply because there is empty space.
Empty space is acceptable when it improves hierarchy.

## AI Behavior
When redesigning an existing UI:
1. Inspect the existing structure first.
2. Identify the current design system.
3. Preserve working functionality.
4. Improve layout and visual hierarchy.
5. Reuse existing components where possible.
6. Avoid unnecessary rewrites.
7. Keep API, WebSocket and device logic unchanged unless explicitly requested.
8. Make the smallest code changes needed to achieve the visual result.
9. Check responsive behavior.
10. Review the final UI against this skill.

Never break functionality merely to make the UI look better.
The final result should feel like a coherent professional product.

## Preferred Visual Direction
Default style: Modern Industrial Dashboard.

Characteristics:
- Neutral dark header
- Light neutral page background
- White or slightly tinted surfaces
- Strong typography
- Compact navigation
- Clean metric cards
- Subtle borders
- Soft shadows
- Controlled status colors
- Minimal decoration
- High information density without feeling crowded

Suitable for energy monitoring, solar systems, hybrid inverters, power meters, industrial controllers, IoT dashboards and ESP32 web interfaces.

When an existing project already has an established visual style, preserve and extend that style rather than replacing it completely.