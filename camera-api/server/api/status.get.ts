export default defineEventHandler(async (event) => {
	const config = useRuntimeConfig(event)

	try {
		return await $fetch(`${config.public.cameraGatewayUrl}/status`, { timeout: 3000 })
	} catch {
		throw createError({
			statusCode: 502,
			statusMessage: 'Camera gateway is unavailable'
		})
	}
})
