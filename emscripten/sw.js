self.addEventListener('install', (event) => {
		event.waitUntil(
				caches.open('v1').then((cache) => cache.addAll([
						'./',
						'./app.html',
						'./RTS.js',
						'./RTS.wasm',
				]))
		)});

self.addEventListener('fetch', (event) => {
		event.respondWith(
				caches.match(event.request).then((response) => response || fetch(event.request))
		);
});

