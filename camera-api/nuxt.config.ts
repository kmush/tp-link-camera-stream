// https://nuxt.com/docs/api/configuration/nuxt-config
export default defineNuxtConfig({
  compatibilityDate: '2025-07-15',
  devtools: { enabled: true },
  runtimeConfig: {
    public: {
      //cameraGatewayUrl: 'http://localhost:8080'
      cameraGatewayUrl: '192.168.1.100/stream1'
    }
  }
})
