"""Shared typing protocols for generator modules."""

from __future__ import annotations

from typing import Generic, Protocol, Sequence, TypeAlias, TypeVar, TYPE_CHECKING

if TYPE_CHECKING:
    from concrete_room import ConcreteRoom
    ConcreteRoomLike: TypeAlias = ConcreteRoom
else:
    class ConcreteRoomLike(Protocol):
        """Fallback concrete room protocol for runtime."""


class TextWriter(Protocol):
    def write(self, string: str) -> int | None:
        """Write a string to an output sink."""


class ElementLike(Protocol):
    def get_class(self) -> str:
        """Return the element class identifier (brick, gate, room)."""

    def get_id(self) -> str:
        """Return the element identifier."""


class SelectableInstance(Protocol):
    def check_fit(self) -> int:
        """Return a truthy weight when the instance matches selection criteria."""


class StructureLike(SelectableInstance, Protocol):
    def get_name(self) -> str:
        """Return the structure name for logging and selection."""

    def check_structure(self) -> bool:
        """Validate structure settings."""

    def instantiate_defaults(self) -> None:
        """Populate structure defaults."""

    def instantiate(self, selector: "SelectorLike") -> None:
        """Instantiate structure parameters."""

    def generic_generate(self, concrete: ConcreteRoomLike) -> None:
        """Generate the structure in a concrete room."""


class DressingLike(SelectableInstance, Protocol):
    def get_name(self) -> str:
        """Return the dressing name for logging and selection."""

    def instantiate(self, selector: "SelectorLike") -> None:
        """Instantiate dressing parameters."""

    def generate(self, concrete: ConcreteRoomLike) -> None:
        """Generate the dressing in a concrete room."""


class SelectorLike(Protocol):
    def load_level_extensions(self, level_directory: str) -> None:
        """Load extension bricks for a given level directory."""

    def get_structure_from_name(self, name: str | None, element: "ElementWithValues") -> StructureLike:
        """Return a structure instance by name."""

    def get_dressing_from_name(self, name: str | None, element: "ElementWithValues") -> DressingLike:
        """Return a dressing instance by name."""

    def get_dressing_fit(self, element: "ElementWithValues") -> list[DressingLike]:
        """Return the dressings that fit the element."""

    def get_random_choice(self, choices: Sequence[DressingLike]) -> DressingLike:
        """Return a random choice from the list."""

    def get_random_int(self, min_value: int, max_value: int) -> int:
        """Return a random integer between bounds (inclusive)."""

    def get_random_float(self, min_value: float, max_value: float) -> float:
        """Return a random float between bounds."""


TSelectable = TypeVar("TSelectable", bound=SelectableInstance, covariant=True)


class InstanceFactory(Protocol, Generic[TSelectable]):
    def get_instance(self, element: "ElementWithValues") -> TSelectable:
        """Return a concrete selectable instance for an element."""


class NamedInstanceFactory(InstanceFactory[TSelectable], Protocol):
    def get_name(self) -> str:
        """Return the registered name for this instance factory."""


class ElementParameters(Protocol):
    structure_class: str | None
    dressing_class: str | None
    dressing_private: dict[str, object] | None
    dressing_parameters: dict[str, object] | None
    structure_private: dict[str, object] | None
    structure_parameters: dict[str, object] | None


class ObjectLike(Protocol):
    type: str
    position: object
    parameters: dict[str, object] | None


class ElementValues(Protocol):
    parameters: ElementParameters
    objects: list[ObjectLike] | None

    def __contains__(self, key: str) -> bool:
        """Allow membership checks for legacy code."""

    def __getitem__(self, key: str) -> object:
        """Allow dict-style access for legacy code."""


TElementValues = TypeVar("TElementValues", bound=ElementValues)


class ElementWithValues(ElementLike, Protocol, Generic[TElementValues]):
    values: TElementValues


class TriggerLike(Protocol):
    trigger_id: str


class PadLike(Protocol):
    pad_id: str
    definition: dict[str, object] | None


class RootPadLike(Protocol):
    ref_b_id: str
    ref_pad_id: str
    translation: list[float] | None
    rotation: dict[str, object] | None


class PortalLike(Protocol):
    gate_id: str
    connect: str


class BrickValues(ElementValues, Protocol):
    b_id: str
    triggers: list[TriggerLike] | None
    pads: list[PadLike] | None
    root_pad: RootPadLike | None
    portals: list[PortalLike] | None


class RoomSpecValues(ElementValues, Protocol):
    room_id: str
    name: str
    triggers: list[TriggerLike] | None
    structure_class: str | None
    dressing_class: str | None


class BrickLike(ElementLike, Protocol):
    values: BrickValues

    def structure_personalization(self) -> None:
        """Instantiate structure parameters."""

    def dressing_instantiation(self) -> None:
        """Instantiate dressing parameters."""

    def dressing_personalization(self) -> None:
        """Personalize dressings."""

    def finalize(self, concrete: ConcreteRoomLike) -> None:
        """Finalize concrete generation for the brick."""

    def dump_graph(self, output_room: TextWriter, output_main: TextWriter, dump_prefix: str) -> None:
        """Dump graphviz data for the brick."""


class RoomValues(ElementValues, Protocol):
    bricks: list[BrickLike]
    human_name: str | None

    def __getitem__(self, key: str) -> object:
        """Allow dict-style access for legacy code."""


class RoomSpecLike(ElementLike, Protocol):
    values: RoomSpecValues
    structure: StructureLike
    dressing: DressingLike

    def save(self, output_directory: str) -> None:
        """Persist the room spec to a directory."""

    def dump_graph(self, output: TextWriter) -> None:
        """Dump graphviz data for the room spec."""

    def structure_personalization(self) -> None:
        """Instantiate structure parameters for the room."""

    def dressing_instantiation(self) -> None:
        """Instantiate dressing parameters for the room."""

    def dressing_personalization(self) -> None:
        """Personalize dressings for the room."""

    def finalize(self, output_directory: str, preview: bool = False) -> None:
        """Finalize room generation."""


class LevelValues(Protocol):
    rooms: list[RoomSpecLike]
    declarations: object
