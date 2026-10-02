<script setup lang="ts">
type CameraStatus = {
  connected: boolean
  recording: boolean
  timestamp: number
}

const { data: status, error, refresh } = useFetch<CameraStatus>('/api/status', {
  default: () => ({ connected: false, recording: false, timestamp: 0 }),
  server: false
})
const commandPending = ref(false)
const commandError = ref('')
let pollTimer: ReturnType<typeof setInterval> | undefined

onMounted(() => {
  pollTimer = setInterval(() => refresh(), 3000)
})

onBeforeUnmount(() => {
  if (pollTimer) clearInterval(pollTimer)
})

async function setRecording(enabled: boolean) {
  commandPending.value = true
  commandError.value = ''
  try {
    await $fetch('/api/cmd', {
      method: 'POST',
      body: { action: enabled ? 'record_on' : 'record_off' }
    })
    await refresh()
  } catch {
    commandError.value = 'Unable to send the recording command.'
  } finally {
    commandPending.value = false
  }
}
</script>

<template>
  <main class="dashboard">
    <header class="topbar">
      <div>
        <p class="eyebrow">VEHICLE CONNECTIVITY</p>
        <h1>Camera Gateway</h1>
      </div>
      <div class="connection" :class="{ online: status?.connected }">
        <span class="dot" />
        {{ status?.connected ? 'Gateway online' : 'Gateway offline' }}
      </div>
    </header>

    <section class="camera-card">
      <div class="card-heading">
        <div>
          <p class="eyebrow">LIVE FEED</p>
          <h2>Front camera</h2>
        </div>
        <span class="live-pill" v-if="status?.connected">LIVE</span>
      </div>
      <div class="video-frame">
        <img v-if="status?.connected" :src="'/api/stream'" alt="Live camera stream">
        <div v-else class="empty-state">
          <span class="camera-icon">◉</span>
          <p>Waiting for camera gateway</p>
          <small>Start the C++ gateway to view the live feed.</small>
        </div>
      </div>
    </section>

    <section class="controls-card">
      <div>
        <p class="eyebrow">RECORDING</p>
        <h2>{{ status?.recording ? 'Recording is on' : 'Recording is off' }}</h2>
        <p class="hint">Recording is saved by the gateway as an AVI file.</p>
      </div>
      <button
        :class="status?.recording ? 'stop-button' : 'record-button'"
        :disabled="commandPending || !status?.connected"
        @click="setRecording(!status?.recording)"
      >
        {{ commandPending ? 'Sending…' : status?.recording ? 'Stop recording' : 'Start recording' }}
      </button>
    </section>
    <p v-if="commandError || error" class="error-message">
      {{ commandError || 'Could not read status from the gateway.' }}
    </p>
  </main>
</template>

<style scoped>
 :global(*) { box-sizing: border-box; }
:global(body) { margin: 0; background: #0b1120; color: #e5eaf4; font-family: Inter, ui-sans-serif, system-ui, sans-serif; }
.dashboard { max-width: 1120px; margin: 0 auto; padding: 48px 28px; }
.topbar, .card-heading, .controls-card { display: flex; align-items: center; justify-content: space-between; gap: 24px; }
.topbar { margin-bottom: 32px; }
.eyebrow { margin: 0 0 8px; color: #8492aa; font-size: 11px; font-weight: 700; letter-spacing: .16em; }
h1, h2, p { margin-top: 0; }
h1 { margin-bottom: 0; font-size: 30px; letter-spacing: -.04em; }
h2 { margin: 0; font-size: 18px; }
.connection { display: flex; align-items: center; gap: 10px; color: #aab4c6; font-size: 13px; }
.dot { width: 9px; height: 9px; border-radius: 50%; background: #ef6672; box-shadow: 0 0 14px #ef667255; }
.connection.online .dot { background: #41d6a0; box-shadow: 0 0 14px #41d6a077; }
.camera-card, .controls-card { border: 1px solid #202b40; border-radius: 18px; background: #111a2b; }
.camera-card { overflow: hidden; }
.card-heading { padding: 22px 26px; }
.live-pill { border-radius: 99px; padding: 6px 10px; color: #ff7b85; background: #632b354f; font-size: 10px; font-weight: 800; letter-spacing: .1em; }
.video-frame { display: grid; min-height: 420px; place-items: center; background: #080d17; }
.video-frame img { display: block; width: 100%; max-height: 68vh; object-fit: contain; }
.empty-state { text-align: center; color: #aab4c6; }
.camera-icon { display: block; margin-bottom: 12px; color: #4a5c78; font-size: 44px; }
.empty-state p { margin-bottom: 6px; color: #dce4f0; font-weight: 600; }
.empty-state small, .hint { color: #8492aa; }
.controls-card { margin-top: 18px; padding: 22px 26px; }
.hint { margin: 8px 0 0; font-size: 13px; }
button { border: 0; border-radius: 10px; padding: 12px 18px; color: #06130f; font-weight: 700; cursor: pointer; }
button:disabled { cursor: not-allowed; opacity: .45; }
.record-button { background: #41d6a0; }
.stop-button { color: #ffe9eb; background: #9f3e4b; }
.error-message { color: #ff9ba3; font-size: 13px; }
@media (max-width: 640px) {
  .dashboard { padding: 28px 16px; }
  .topbar, .controls-card { align-items: flex-start; flex-direction: column; }
  .video-frame { min-height: 240px; }
}
</style>
