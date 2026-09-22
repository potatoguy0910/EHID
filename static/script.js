function showSection(sectionId) {

    document.querySelectorAll(".section").forEach(section => {
        section.classList.remove("active-section");
    });

    document.getElementById(sectionId)
        .classList.add("active-section");

    document.querySelectorAll(".nav-item").forEach(item => {
        item.classList.remove("active");
    });

    if (sectionId === "dashboard") {
        document.getElementById("page-title").innerText =
            "Emergency Health ID";
    }

    if (sectionId === "scan") {
        document.getElementById("page-title").innerText =
            "Scan Fingerprint";
    }

    if (sectionId === "enroll") {
        document.getElementById("page-title").innerText =
            "Enroll Fingerprint";
    }
}


// ================= SCAN =================

async function startScan() {

    const button = document.getElementById("scan-button");
    const status = document.getElementById("scan-status");
    const message = document.getElementById("scan-message");

    button.disabled = true;

    status.innerText = "Scanning...";
    message.innerText =
        "Place your finger on the R307S sensor.";

    document
        .getElementById("patient-result")
        .classList.add("hidden");

    await fetch("/api/scan", {
        method: "POST"
    });

    checkScanStatus();
}


async function checkScanStatus() {

    const response = await fetch("/api/status");
    const data = await response.json();

    const status = document.getElementById("scan-status");
    const message = document.getElementById("scan-message");
    const button = document.getElementById("scan-button");

    if (data.status === "scanning") {

        status.innerText = "Identifying...";
        message.innerText =
            "Searching fingerprint records...";

        setTimeout(checkScanStatus, 500);

        return;
    }


    if (data.status === "identified") {

        status.innerText = "Identity Verified";
        message.innerText =
            "Patient record successfully retrieved.";

        displayPatient(data);

        button.disabled = false;

        return;
    }


    if (data.status === "not_found") {

        status.innerText = "No Match Found";

        message.innerText =
            "This fingerprint is not registered.";

        button.disabled = false;

        return;
    }
}


function displayPatient(data) {

    const patient = data.patient;

    document.getElementById("patient-name").innerText =
        patient.name;

    document.getElementById("patient-id").innerText =
        "#" + String(data.fingerprint_id).padStart(3, "0");

    document.getElementById("blood-group").innerText =
        patient.blood_group;

    document.getElementById("allergies").innerText =
        patient.allergies;

    document.getElementById("conditions").innerText =
        patient.conditions;

    document.getElementById("surgeries").innerText =
        patient.surgeries;

    document.getElementById("medications").innerText =
        patient.medications;

    document.getElementById("emergency-contact").innerText =
        patient.emergency_contact;

    document
        .getElementById("patient-result")
        .classList.remove("hidden");
}


// ================= ENROLL =================

async function startEnrollment() {

    const id =
        document.getElementById("enroll-id").value;

    const status =
        document.getElementById("enroll-status");

    if (!id || id < 1 || id > 127) {

        status.innerText =
            "Please enter a valid fingerprint ID.";

        return;
    }

    status.innerText =
        "Starting enrollment...";

    const response = await fetch(
        `/api/enroll/${id}`,
        {
            method: "POST"
        }
    );

    const data = await response.json();

    if (data.status === "started") {

        status.innerText =
            "Place your finger on the sensor.";

        monitorEnrollment();
    }
}


async function monitorEnrollment() {

    const response =
        await fetch("/api/enrollment-status");

    const data =
        await response.json();

    const status =
        document.getElementById("enroll-status");

    status.innerText =
        data.message;

    if (!data.complete) {

        setTimeout(
            monitorEnrollment,
            500
        );

    } else {

        status.innerText =
            "✓ Fingerprint enrolled successfully!";
    }
}