// We actually only neeed WORKERFS and FS from there,
// for simplicity sake. This is not optimal.
// Try to figure out a way to get rid of this
importScripts('RTS.js');
//importScripts('libworkerfs');

console.log("Hello from worker")

const handleMap = new Map();
var handleCounter = 0;

// Assign an ID to a create object, for later reference
function CreateHandleFromResponse(response)
{
  console.log("Creating handle with id " + handleCounter);
  handleMap.set(handleCounter, response);
  handleCounter = handleCounter + 1;
}

self.onmessage = function(e) {
  console.log("Worker received message:");
  console.log(e.data);
  if (e.data.type === 'mount') {
    const files = e.data.files;

    // 1. Ordner erstellen (falls noch nicht vorhanden)
    if (!FS.analyzePath('/data').exists) {
      FS.mkdir('/data');
    }

    // 2. WORKERFS mounten
    var ret = FS.mount(WORKERFS, {
      files: files
    }, '/data');

    console.log("Ret");
    console.log(ret);
    //self.CreateHandleFromResponse(ret);
    //self.postMessage({type: "mountResponse", ret: ret})

    if (!FS.analyzePath('/idbfs').exists) {
      FS.mkdir('/idbfs');
    }

    ret = FS.mount(IDBFS, {}, '/idbfs');

    console.log("Ret of IDBFS mount " + ret);

    function copyRecursive(srcDir, dstDir) {
      if (!FS.analyzePath(dstDir).exists) {
      FS.mkdir(dstDir);
      }
      var entries = FS.readdir(srcDir);
      entries = entries.filter(e => e !== '.' && e !== '..');
      for (const name of entries) {
      const srcPath = srcDir + '/' + name;
      const dstPath = dstDir + '/' + name;
      const stat = FS.stat(srcPath);
      if (FS.isDir(stat.mode)) {
        console.log(`Dir: ${srcPath}`);
        copyRecursive(srcPath, dstPath);
      } else {
        const data = FS.readFile(srcPath);
        FS.writeFile(dstPath, data);
        console.log(`Copied: ${srcPath} -> ${dstPath}`);
      }
      }
    }

    copyRecursive('/data', '/idbfs');
    FS.syncfs(false, function(err) {
      if (err) {
      console.error('IDBFS sync error:', err);
      } else {
      console.log('IDBFS sync complete');
      }
    });
  }
  if (e.data.type === "createNode") {
    const createNodeOpts = e.data.createNodeOpts;

    FS.createNode(createNodeOpts.parent, createNodeOpts.name, createNodeOpts.mode, createNodeOpts.dev, createNodeOpts.content, createNodeOpts.mtime);

    console.log("Ret createNode");
    console.log(ret);
    self.CreateHandleFromResponse(ret);
  }
  if (e.data.type === 'init') {
    console.log("Init received, answering");
    self.postMessage({type: "initAck"})
  }
  console.log("Message from main received:")
  console.log(e)
  console.log("worker context: " + self)
};

self.onRuntimeInitialized = function() {
  statusEl.textContent = "Engine geladen – bitte Spieldaten auswählen";
}
