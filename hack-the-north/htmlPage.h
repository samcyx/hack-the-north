#ifndef HTML_PAGE_H
#define HTML_PAGE_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Basket Bot</title>
<link href="https://cdnjs.cloudflare.com/ajax/libs/meyer-reset/2.0/reset.min.css" rel="stylesheet">
<style>
  * { box-sizing: border-box; -webkit-font-smoothing: antialiased; }
  body { margin:0; height:100%; font-family: Helvetica, Arial, sans-serif; background:#111; display:flex; justify-content:center; align-items:center; }

  .iphone { display:grid; justify-items:center; align-items:start; width:100vw; }
  .iphone .div { width:393px; height:852px; position:relative; background:linear-gradient(174deg, rgba(38,33,32,1) 0%, rgba(33,29,28,1) 100%); border-radius:40px; padding:20px; display:flex; flex-direction:column; align-items:center; }

  h1 { color:white; font-size:2.5rem; margin:10px 0; text-align:center; }

  .levels { display:flex; justify-content:center; gap:10px; margin-bottom:20px; flex-wrap:wrap; }
  .level-ball { width:40px; height:40px; border-radius:12px; background:#444; color:white; display:flex; justify-content:center; align-items:center; cursor:pointer; }
  .level-ball.active { background:crimson; box-shadow:0 0 10px crimson; }

  .scoreboard { width:100%; display:flex; flex-direction:column; gap:20px; align-items:center; }
  .team { width:80%; padding:20px; border-radius:20px; background:linear-gradient(145deg,#333,#222); display:flex; justify-content:space-between; align-items:center; color:white; box-shadow:0 4px 10px rgba(0,0,0,0.5); }
  .team h2 { margin:0; font-size:1.2rem; }
  .team h3 { font-size:4rem; color:#ff3232; margin:0; }
  .team .buttons { display:flex; flex-direction:column; gap:10px; }
  .team .buttons button { padding:5px 10px; border-radius:10px; border:none; background:#555; color:white; cursor:pointer; font-size:1.2rem; }
  .team .buttons button:hover { background:#777; }

  .reset-btn { margin-top:15px; padding:8px 15px; border-radius:12px; border:none; background:#555; color:white; cursor:pointer; }
  .reset-btn:hover { background:#777; }

  @media(max-width:450px){
    .iphone .div { width:90vw; height:auto; }
    .team h3 { font-size:3rem; }
    .level-ball { width:30px; height:30px; }
  }

  /* Winner overlay */
  .winner-overlay {
    position:fixed; top:0; left:0; right:0; bottom:0;
    display:flex; justify-content:center; align-items:center;
    background:#111;
    z-index:999;
  }
  .winner-card {
    background: linear-gradient(145deg,#333,#222);
    padding:40px 60px;
    border-radius:25px;
    color:white;       /* winner text is white */
    font-family: 'Helvetica', sans-serif;
    font-size:3rem;
    text-align:center;
  }
</style>
</head>
<body>
  <div class="iphone">
    <div class="div" id="mainUI">
      <h1>Basket Bot</h1>

      <div class="levels">
        <div class="level-ball" id="level1">1</div>
        <div class="level-ball" id="level2">2</div>
        <div class="level-ball" id="level3">3</div>
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

      <button class="reset-btn" onclick="resetScores()">Reset Points</button>
    </div>
  </div>

  <!-- Winner Overlay -->
  <div class="winner-overlay" id="winnerOverlay" style="display:none">
    <div class="winner-card" id="winnerText">Winner</div>
  </div>

  <script>
    function updateUI(data){
      document.getElementById('pinkScore').innerText = data.pinkScore;
      document.getElementById('whiteScore').innerText = data.whiteScore;

      // Update levels
      for(let i=1;i<=3;i++){
        document.getElementById('level'+i).classList.remove('active');
      }
      document.getElementById('level'+data.currentLevel).classList.add('active');

      // Show winner overlay if game over
      if(data.gameOver === true){
        document.getElementById('mainUI').style.display = 'none';
        document.getElementById('winnerOverlay').style.display = 'flex';
        document.getElementById('winnerText').innerText = data.finalWinner + " wins!";
      } else {
        document.getElementById('mainUI').style.display = 'flex';
        document.getElementById('winnerOverlay').style.display = 'none';
      }
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

    setInterval(pollServer,1000);
  </script>
</body>
</html>
)rawliteral";

#endif
