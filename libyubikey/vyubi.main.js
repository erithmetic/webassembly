window.tokens = loadTokens()

function vyubiMain() {
  listTokens()
}

function randomInt(max) {
  return Math.floor(Math.random() * Math.floor(max))
}

function randomChars(chars, length) {
  let str = []
  for(let x = 0; x < length; x++) {
    let hex = randomInt(chars.length)
    str = str.concat([chars[hex]])
  }
  return str.join('')
}

function randomModhex(length) {
  const modHexes = ['c','b','d','e','f','g','h','i','j','k','l','n','r','t','u','v']
  return randomChars(modHexes, length)
}

function randomHexes(length) {
  const hexes = ['0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f']
  return randomChars(hexes, length)
}

function addToken(name) {
  otp = {
    name,
    serial: randomInt(9999999),
    publicId: randomModhex(12),
    privateId: randomHexes(12),
    aes: randomHexes(32),
    counter: 0
  }
  window.tokens = [...window.tokens, otp]
}

function loadTokens() {
  let data = localStorage.getItem('tokens')
  if (data) {
    let tokens = JSON.parse(data)
    return tokens
  } else {
    return []
  }
}

function saveTokens() {
  let json = JSON.stringify(window.tokens)
  localStorage.setItem('tokens', json)
}

function listTokens() {
  let container = document.getElementById('tokens')
  while (container.firstChild) {
    container.removeChild(container.firstChild);
  }

  for(let x = 0; x < window.tokens.length; x++) {
    let token = window.tokens[x]
    let entry = document.createElement('li', { })
    entry.id = token.aes
    entry.className = 'token_entry'

    let description = document.createElement('span')
    description.innerText = token.name + " " + token.serial
    entry.appendChild(description)

    let button = document.createElement('button')
    button.innerText = 'Generate OTP'
    button.onclick = function() {
      onGenerateOtp(token)
    }
    entry.appendChild(button)

    let otp = document.createElement('pre')
    otp.id = token.aes + '_otp'
    entry.appendChild(otp)

    container.appendChild(entry)
  }
}

function paddedHex(intVal, width) {
  let hexVal = intVal.toString(16)
  return hexVal.length >= width ? hexVal : new Array(width - hexVal.length + 1).join('0') + hexVal;
}

function generateOtp(token) {
  token.counter = token.counter + 1
  saveTokens()
  let counterHex = paddedHex(token.counter, 4)
  console.log("OTP for ", token, counterHex)
  return Module.ccall('ykgenerate', 'string', 
    ['string', 'string', 'string', 'string', 'string', 'string'], 
    [token.aes, token.privateId, counterHex, '0000', '00', '00']
  )
}

function onAddTokenClick() {
  const name = document.getElementById('name')
  addToken(name.value)
  name.value = ''
  saveTokens()
  listTokens()
}

function onGenerateOtp(token) {
  let otp = generateOtp(token)
  document.getElementById(token.aes + '_otp').innerText = token.publicId + otp
}

function onDownloadCsvClick() {
  let rows = [];
  window.tokens.forEach(function(token) {
    rows = rows.concat([[
      token.serial.toString(),
      token.publicId,
      token.privateId,
      token.aes,
      '000000000000',
      '2018-08-14T08:13:05'
    ]])
  })

  let csvContent = "data:text/csv;charset=utf-8,";
  rows.forEach(function(rowArray){
    let row = rowArray.join(",")
    csvContent += row + "\r\n"
  }) 
  let encodedUri = encodeURI(csvContent);
  let link = document.createElement("a");
  link.setAttribute('href', encodedUri);
  link.setAttribute('download', 'ykksm.csv');
  link.setAttribute('style', 'display: none')
  document.body.appendChild(link); // Required for FF

  link.click();
}