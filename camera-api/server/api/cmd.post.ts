export default defineEventHandler(async (event) => {
	const { action } = await readBody<{ action?: string }>(event)

	if (action !== 'record_on' && action !== 'record_off') {
		throw createError({ statusCode: 400, statusMessage: 'Expected record_on or record_off' })
	}

	const config = useRuntimeConfig(event)
	try {
		return await $fetch(`${config.public.cameraGatewayUrl}/record`, {
			method: 'POST',
			body: action === 'record_on' ? 'on' : 'off',
			headers: { 'content-type': 'text/plain' },
			timeout: 3000
		})
	} catch {
		throw createError({
			statusCode: 502,
			statusMessage: 'Could not send command to camera gateway'
		})
	}
})
