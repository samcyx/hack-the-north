#ifndef HTML_PAGE_H
#define HTML_PAGE_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Basket Bot</title>
  <style>
    body {
      margin:0;
      font-family: Arial, sans-serif;
      background: black;
      color: white;
      text-align: center;
    }

    h1 {
      font-size: 2rem;
      color: crimson;
      padding: 10px 20px;
      border: 2px solid crimson;
      border-radius: 12px;
      display: inline-block;
      margin-top: 15px;
      text-shadow: 0 0 10px crimson;
    }

    .scoreboard {
      display: flex;
      flex-direction: row;
      justify-content: center;
      gap: 15px;
      flex-wrap: wrap;
      margin: 20px 0;
    }

    .team {
      flex:1 1 120px;
      padding: 15px;
      border-radius: 12px;
      box-shadow: 0 0 12px rgba(255,255,255,0.2);
      margin:5px;
    }

    .pink {
      background: hotpink;
      color: white;
      text-shadow: 0 0 8px white;
    }

    .white {
      background: white;
      color: black;
      text-shadow: 0 0 8px black;
    }

    h2 { margin:5px; font-size:1.2rem;}
    h3 { margin:5px; font-size:1.5rem;}

    .buttons button {
      background: crimson;
      border: none;
      padding: 8px 12px;
      margin: 5px 2px;
      border-radius: 8px;
      font-size: 1rem;
      color:white;
      cursor:pointer;
      transition:0.2s;
    }

    .buttons button:hover {
      background: darkred;
    }

    .last-ball {
      margin: 15px auto;
      padding: 10px 15px;
      border: 2px solid crimson;
      border-radius: 12px;
      width: fit-content;
      background: #222;
      box-shadow: 0 0 12px crimson;
      font-size: 1.2rem;
    }

    .level-indicator {
      margin: 15px 0;
    }

    .levels {
      display: flex;
      justify-content: center;
      gap: 8px;
      flex-wrap: wrap;
    }

    .level-ball {
      width: 25px;
      height: 25px;
      border-radius: 50%;
      border: 2px solid white;
      background: #555;
      opacity:0.5;
    }

    .level-ball.active {
      background: crimson;
      opacity:1;
      box-shadow: 0 0 10px crimson;
    }

    .message {
      margin-top: 12px;
      font-size: 1.2rem;
      color: yellow;
      text-shadow: 0 0 10px yellow;
      min-height:1.5em;
    }

    @media (max-width:500px){
      h1 { font-size:1.5rem;}
      .scoreboard { flex-direction: column;}
      .team { margin:5px auto; width:80%;}
      .buttons button{ font-size:0.9rem; padding:6px 10px;}
      .level-ball { width:20px; height:20px;}
    }
  </style>
</head>
<body>
  <h1>Basket Bot</h1>

  <div class="level-indicator">
    <div class="levels">
      <div class="level-ball" id="level1"></div>
      <div class="level-ball" id="level2"></div>
      <div class="level-ball" id="level3"></div>
      <div class="level-ball" id="level4"></div>
      <div class="level-ball" id="level5"></div>
    </div>
  </div>

  <div class="scoreboard">
    <div class="team pink">
      <h2>Pink Team</h2>
      <h3 id="pinkScore">0</h3>
      <div class="buttons">
        <button onclick="updateScore('pink',1)">+</button>
        <button onclick="updateScore('pink',-1)">-</button>
      </div>
    </div>
    <div class="team white">
      <h2>White Team</h2>
      <h3 id="whiteScore">0</h3>
      <div class="buttons">
        <button onclick="updateScore('white',1)">+</button>
        <button onclick="updateScore('white',-1)">-</button>
      </div>
    </div>
  </div>

  <div class="last-ball">
    Last Ball Winner: <span id="lastWinner">-</span>
  </div>

  <div class="buttons">
    <button onclick="resetScores()">Reset Points</button>
  </div>

  <div class="message" id="message"></div>

  <script>
    function updateUI(data){
      document.getElementById('pinkScore').innerText = data.pinkScore;
      document.getElementById('whiteScore').innerText = data.whiteScore;
      document.getElementById('lastWinner').innerText = data.lastWinner;
      document.getElementById('message').innerText = data.message;

      // Levels
      for(let i=1;i<=5;i++){
        document.getElementById('level'+i).classList.remove('active');
      }
      document.getElementById('level'+data.currentLevel).classList.add('active');
    }

    function updateScore(team,delta){
      fetch('/updateScore?team='+team+'&delta='+delta)
        .then(resp=>resp.json())
        .then(data=>updateUI(data));
    }

    function resetScores(){
      fetch('/reset')
        .then(resp=>resp.json())
        .then(data=>updateUI(data));
    }

    function pollServer(){
      fetch('/updateScore?team=none&delta=0')
        .then(resp=>resp.json())
        .then(data=>updateUI(data))
        .catch(err=>console.log(err));
    }

    setInterval(pollServer,1000); // Auto-update every 1 sec
  </script>
</body>
</html>
)rawliteral";

#endif
