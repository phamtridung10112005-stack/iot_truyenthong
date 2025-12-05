const btnConnect = document.getElementById('btnConnect');
const btnDisconnect = document.getElementById('btnDisconnect');
const statusText = document.getElementById('status');
const tempVal = document.getElementById('tempVal');
const humVal = document.getElementById('humVal');

let intervalId = null;

// Hàm cập nhật giao diện khi kết nối thành công/thất bại
function setConnected(isConnected) {
    if (isConnected) {
        btnConnect.classList.add('hidden');
        btnDisconnect.classList.remove('hidden');
        statusText.textContent = "Trạng thái: Đang nhận dữ liệu...";
        statusText.style.color = "green";
        
        // Bắt đầu hỏi server dữ liệu mỗi 1 giây
        startFetching();
    } else {
        btnConnect.classList.remove('hidden');
        btnDisconnect.classList.add('hidden');
        statusText.textContent = "Trạng thái: Đã ngắt kết nối";
        statusText.style.color = "red";
        
        stopFetching();
    }
}

// Hàm gọi API lấy dữ liệu liên tục (Polling)
function startFetching() {
    intervalId = setInterval(async () => {
        try {
            const res = await fetch('/data');
            const data = await res.json();
            
            // Cập nhật số lên màn hình
            tempVal.innerText = data.temp;
            humVal.innerText = data.hum;
        } catch (err) {
            console.error(err);
        }
    }, 1000); // 1000ms = 1 giây
}

function stopFetching() {
    clearInterval(intervalId);
    tempVal.innerText = "0";
    humVal.innerText = "0";
}

// --- SỰ KIỆN NÚT BẤM ---
btnConnect.addEventListener('click', async () => {
    try {
        const res = await fetch('/connect', { method: 'POST' });
        const result = await res.json();
        if (result.status === 'OK') {
            setConnected(true);
        } else {
            alert('Lỗi kết nối: ' + result.msg);
        }
    } catch (err) {
        alert('Không gọi được Server!');
    }
});

btnDisconnect.addEventListener('click', async () => {
    await fetch('/disconnect', { method: 'POST' });
    setConnected(false);
});