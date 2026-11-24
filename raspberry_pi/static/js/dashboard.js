const $=id=>document.getElementById(id);
const fmt=(v,n=2)=>Number.isFinite(Number(v))?Number(v).toFixed(n):"—";
async function latest(){try{
 const r=await fetch("/api/data",{cache:"no-store"});if(!r.ok)throw Error();const d=await r.json();
 $("connection").textContent="Server connected";if(!d.id)return;
 const accident=Number(d.accident)===1;$("accidentStatus").textContent=accident?"Potential accident detected":"No accident flag";
 $("accidentStatus").style.color=accident?"#ff7777":"#8ce0a4";$("receivedAt").textContent="Last received: "+d.received_at;
 ["ax","ay","az"].forEach(k=>$(k).textContent=fmt(d[k]));$("speed").textContent=fmt(d.speed,1);
 $("satellites").textContent=d.satellites;$("sequence").textContent=d.sequence;
 const valid=Number(d.gps_valid)===1;$("gps").textContent=valid?`${d.lat}, ${d.lng}`:"No valid GPS fix";
 const a=$("mapLink");a.hidden=!valid;if(valid)a.href=`https://www.google.com/maps?q=${encodeURIComponent(d.lat)},${encodeURIComponent(d.lng)}`;
 }catch(e){$("connection").textContent="Server unavailable"}}
async function history(){try{const r=await fetch("/api/history?limit=20");const rows=await r.json();const body=$("history");body.replaceChildren();
 if(!rows.length){body.innerHTML='<tr><td colspan="5">No records yet</td></tr>';return}
 rows.forEach(d=>{const tr=document.createElement("tr");[d.received_at,d.sequence,Number(d.accident)?"Potential accident":"Normal",fmt(d.speed,1)+" km/h",Number(d.gps_valid)?`${d.lat}, ${d.lng}`:"No fix"].forEach(v=>{const td=document.createElement("td");td.textContent=v??"";tr.appendChild(td)});body.appendChild(tr)})}catch(e){}}
latest();history();setInterval(latest,1000);setInterval(history,5000);
