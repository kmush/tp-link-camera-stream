import { sendRedirect } from 'h3'

export default defineEventHandler((event) => {
	const config = useRuntimeConfig(event)
	return sendRedirect(event, `${config.public.cameraGatewayUrl}/stream`, 307)
})
