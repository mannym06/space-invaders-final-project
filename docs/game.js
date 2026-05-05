/*
  Space Invaders - Browser Version
  --------------------------------
  Team Members:
  - Nabila
  - Justin
  - Denim
  - Manuel

  Browser version team responsibility:
  - Manuel: game state, score, lives, level, and collision rules
  - Nabila: enemy and shield setup
  - Justin: keyboard input, movement, and shooting logic
  - Denim: drawing, colors, HUD, restart screen, and page layout

  This file mirrors the C++ SFML version but uses JavaScript and HTML5 Canvas.
  It is included so the game can be played online through GitHub Pages.
*/

const canvas = document.getElementById("gameCanvas");
const ctx = canvas.getContext("2d");
const restartButton = document.getElementById("restartButton");

const WIDTH = canvas.width;
const HEIGHT = canvas.height;

const keys = new Set();

const player = {
  x: WIDTH / 2 - 29,
  y: HEIGHT - 70,
  width: 58,
  height: 22,
  speed: 360,
};

let enemies = [];
let bullets = [];
let shields = [];
let score = 0;
let lives = 3;
let level = 1;
let enemyDirection = 1;
let enemySpeed = 45;
let playerShootCooldown = 0;
let enemyShootTimer = 0;
let gameState = "playing";
let lastTime = performance.now();

function resetGame() {
  player.x = WIDTH / 2 - player.width / 2;
  player.y = HEIGHT - 70;
  score = 0;
  lives = 3;
  level = 1;
  enemyDirection = 1;
  enemySpeed = 45;
  playerShootCooldown = 0;
  enemyShootTimer = 0;
  gameState = "playing";
  bullets = [];
  createEnemies();
  createShields();
}

function startNextLevel() {
  level += 1;
  enemySpeed += 16;
  enemyDirection = 1;
  bullets = [];
  gameState = "playing";
  createEnemies();
  createShields();
}

function createEnemies() {
  enemies = [];
  const startX = 130;
  const startY = 80;
  const gapX = 58;
  const gapY = 44;

  for (let row = 0; row < 5; row += 1) {
    for (let col = 0; col < 11; col += 1) {
      enemies.push({
        x: startX + col * gapX,
        y: startY + row * gapY,
        width: 42,
        height: 26,
        alive: true,
        points: (5 - row) * 10,
      });
    }
  }
}

function createShields() {
  shields = [];
  const centers = [180, 360, 540, 720];
  const shieldY = HEIGHT - 190;
  const blockWidth = 14;
  const blockHeight = 10;

  for (const centerX of centers) {
    for (let row = 0; row < 4; row += 1) {
      for (let col = 0; col < 7; col += 1) {
        const bottomHole = row === 3 && (col === 2 || col === 3 || col === 4);
        if (bottomHole) continue;

        shields.push({
          x: centerX - 49 + col * blockWidth,
          y: shieldY + row * blockHeight,
          width: blockWidth,
          height: blockHeight,
          alive: true,
        });
      }
    }
  }
}

function rectsOverlap(a, b) {
  return a.x < b.x + b.width &&
    a.x + a.width > b.x &&
    a.y < b.y + b.height &&
    a.y + a.height > b.y;
}

function clamp(value, min, max) {
  return Math.max(min, Math.min(value, max));
}

function update(deltaTime) {
  if (keys.has("r")) {
    resetGame();
    return;
  }

  if (gameState !== "playing") return;

  playerShootCooldown = Math.max(0, playerShootCooldown - deltaTime);
  enemyShootTimer -= deltaTime;

  updatePlayer(deltaTime);
  updateEnemies(deltaTime);
  updateBullets(deltaTime);
  handleCollisions();
  enemyShoot();
  checkWinOrLose();
}

function updatePlayer(deltaTime) {
  let movement = 0;

  if (keys.has("arrowleft") || keys.has("a")) {
    movement -= player.speed * deltaTime;
  }

  if (keys.has("arrowright") || keys.has("d")) {
    movement += player.speed * deltaTime;
  }

  player.x = clamp(player.x + movement, 20, WIDTH - player.width - 20);

  if (keys.has(" ") && playerShootCooldown <= 0) {
    bullets.push({
      x: player.x + player.width / 2 - 3,
      y: player.y - 16,
      width: 6,
      height: 16,
      speed: -560,
      fromPlayer: true,
      active: true,
    });
    playerShootCooldown = 0.28;
  }
}

function updateEnemies(deltaTime) {
  let shouldDrop = false;

  for (const enemy of enemies) {
    if (!enemy.alive) continue;

    const nextX = enemy.x + enemyDirection * enemySpeed * deltaTime;
    if (nextX < 20 || nextX + enemy.width > WIDTH - 20) {
      shouldDrop = true;
      break;
    }
  }

  if (shouldDrop) {
    enemyDirection *= -1;
    for (const enemy of enemies) {
      enemy.y += 26;
    }
  } else {
    for (const enemy of enemies) {
      enemy.x += enemyDirection * enemySpeed * deltaTime;
    }
  }
}

function updateBullets(deltaTime) {
  for (const bullet of bullets) {
    bullet.y += bullet.speed * deltaTime;
  }

  bullets = bullets.filter((bullet) => bullet.active && bullet.y > -30 && bullet.y < HEIGHT + 30);
}

function enemyShoot() {
  if (enemyShootTimer > 0) return;

  const livingEnemies = enemies.filter((enemy) => enemy.alive);
  if (livingEnemies.length === 0) return;

  const shooter = livingEnemies[Math.floor(Math.random() * livingEnemies.length)];
  bullets.push({
    x: shooter.x + shooter.width / 2 - 3,
    y: shooter.y + shooter.height + 4,
    width: 6,
    height: 16,
    speed: 250 + level * 18,
    fromPlayer: false,
    active: true,
  });

  enemyShootTimer = 0.35 + Math.random() * 0.6;
}

function handleCollisions() {
  for (const bullet of bullets) {
    if (!bullet.active) continue;

    if (bullet.fromPlayer) {
      for (const enemy of enemies) {
        if (enemy.alive && rectsOverlap(bullet, enemy)) {
          enemy.alive = false;
          bullet.active = false;
          score += enemy.points;
          break;
        }
      }
    } else if (rectsOverlap(bullet, player)) {
      bullet.active = false;
      lives -= 1;
      player.x = WIDTH / 2 - player.width / 2;

      if (lives <= 0) {
        gameState = "lost";
      }
    }

    for (const block of shields) {
      if (block.alive && bullet.active && rectsOverlap(bullet, block)) {
        block.alive = false;
        bullet.active = false;
        break;
      }
    }
  }
}

function checkWinOrLose() {
  const allEnemiesDefeated = enemies.every((enemy) => !enemy.alive);
  if (allEnemiesDefeated) {
    startNextLevel();
    return;
  }

  for (const enemy of enemies) {
    if (enemy.alive && enemy.y + enemy.height >= player.y) {
      gameState = "lost";
      return;
    }
  }
}

function draw() {
  ctx.clearRect(0, 0, WIDTH, HEIGHT);
  ctx.fillStyle = "#080a18";
  ctx.fillRect(0, 0, WIDTH, HEIGHT);

  drawStars();
  drawHud();
  drawPlayer();
  drawEnemies();
  drawShields();
  drawBullets();
  drawMessages();
}

function drawStars() {
  ctx.fillStyle = "#a0a0be";
  for (let i = 0; i < 70; i += 1) {
    const x = (i * 137) % WIDTH;
    const y = (i * 83) % (HEIGHT - 100);
    ctx.fillRect(x, y, 2, 2);
  }
}

function drawHud() {
  ctx.fillStyle = "#f4f7ff";
  ctx.font = "20px Arial";
  ctx.fillText(`Score: ${score}`, 20, 34);
  ctx.fillText(`Lives: ${lives}`, 400, 34);
  ctx.fillText(`Level: ${level}`, 780, 34);

  ctx.fillStyle = "#46aaff";
  ctx.fillRect(0, HEIGHT - 32, WIDTH, 3);
}

function drawPlayer() {
  ctx.fillStyle = "#3cd273";
  ctx.fillRect(player.x, player.y, player.width, player.height);

  ctx.fillStyle = "#66f096";
  ctx.fillRect(player.x + player.width / 2 - 6, player.y - 14, 12, 18);
}

function drawEnemies() {
  for (const enemy of enemies) {
    if (!enemy.alive) continue;

    ctx.fillStyle = "#d250f0";
    ctx.fillRect(enemy.x, enemy.y, enemy.width, enemy.height);

    ctx.fillStyle = "#080a18";
    ctx.fillRect(enemy.x + 9, enemy.y + 8, 6, 6);
    ctx.fillRect(enemy.x + enemy.width - 15, enemy.y + 8, 6, 6);
  }
}

function drawShields() {
  ctx.fillStyle = "#46aaff";
  for (const block of shields) {
    if (block.alive) {
      ctx.fillRect(block.x, block.y, block.width, block.height);
    }
  }
}

function drawBullets() {
  for (const bullet of bullets) {
    ctx.fillStyle = bullet.fromPlayer ? "#ffeb5a" : "#ff5a5a";
    ctx.fillRect(bullet.x, bullet.y, bullet.width, bullet.height);
  }
}

function drawMessages() {
  if (gameState !== "lost") return;

  ctx.fillStyle = "rgba(0, 0, 0, 0.72)";
  ctx.fillRect(0, 0, WIDTH, HEIGHT);

  ctx.fillStyle = "#ffffff";
  ctx.textAlign = "center";
  ctx.font = "54px Arial";
  ctx.fillText("GAME OVER", WIDTH / 2, HEIGHT / 2 - 20);
  ctx.font = "24px Arial";
  ctx.fillText("Press R or click Restart to play again", WIDTH / 2, HEIGHT / 2 + 28);
  ctx.textAlign = "left";
}

function gameLoop(currentTime) {
  const deltaTime = Math.min((currentTime - lastTime) / 1000, 0.05);
  lastTime = currentTime;

  update(deltaTime);
  draw();
  requestAnimationFrame(gameLoop);
}

document.addEventListener("keydown", (event) => {
  keys.add(event.key.toLowerCase());

  if ([" ", "arrowleft", "arrowright"].includes(event.key.toLowerCase())) {
    event.preventDefault();
  }
});

document.addEventListener("keyup", (event) => {
  keys.delete(event.key.toLowerCase());
});

restartButton.addEventListener("click", resetGame);

resetGame();
requestAnimationFrame(gameLoop);
