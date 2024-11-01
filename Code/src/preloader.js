function showPreloader() {
    // แสดง preloader
    document.getElementById('preloader').classList.add('show');
}

function hidePreloader() {
    // ซ่อน preloader
    document.getElementById('preloader').classList.remove('show');
}

// ฟังก์ชันนี้ใช้ในการเปลี่ยนหน้าไปยังลิงก์ที่กำหนดพร้อมกับการแสดง preloader
function navigateWithPreloader(url) {
    showPreloader();
    setTimeout(function() {
        window.location.href = url;
    }, 2000); // เวลาในการแสดง preloader (2 วินาที)
}

// ซ่อน preloader อัตโนมัติเมื่อหน้าโหลดเสร็จ
window.addEventListener("load", hidePreloader);

