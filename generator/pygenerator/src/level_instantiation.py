
from __future__ import annotations

from typing import Any

import logging
import langchain
import langgraph

# Only support the Ollama backend
BACKEND: str | None
try:
    from langchain_ollama import OllamaLLM
    BACKEND = "ollama"
except Exception:
    BACKEND = None

logging.basicConfig()
logger = logging.getLogger("generator")
logger.setLevel(logging.INFO)


def instantiate() -> dict[str, Any]:
    """Initialize a local Llama model and return a tiny completion plus versions.

    Uses Ollama if available (requires the Ollama daemon and a pulled model, e.g., `llama3`),
    otherwise falls back to llama-cpp-python if installed with a local GGUF model.
    """

    versions = {
        "langchain": getattr(langchain, "__version__", "unknown"),
        "langgraph": getattr(langgraph, "__version__", "unknown"),
    }
    logger.info("Langchain version: %s", versions["langchain"])
    logger.info("Langgraph version: %s", versions["langgraph"])

    result: dict[str, Any] = {
        "backend": BACKEND or "none",
        "versions": versions,
        "sample": None,
    }

    if BACKEND == "ollama":
        # Model name must exist locally, e.g., `ollama pull llama3`
        llm = OllamaLLM(model="llama3", temperature=0.2)
        result["sample"] = llm.invoke("genere un fichier gltf parfaitement valide représentant une salle de séjour simplifiée avec 4 portes")
        logger.info("Ollama result: %s", result)
    
    else:
        logger.warning("Aucun backend Llama local disponible (installez ollama ou llama-cpp-python)")

    return result

