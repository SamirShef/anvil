use std::collections::HashMap;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct InstId(pub usize);

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct BasicBlockId(pub usize);

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct FunctionId(pub usize);

pub enum Inst {
    Binary(InstId, InstId),
    Call(FunctionId),
    Jump(BasicBlockId),
    JumpIf(InstId, BasicBlockId, BasicBlockId),
    Alloca,
    Load(InstId),
    Store(InstId),
}

pub struct BasicBlock {
    pub body: Vec<Inst>,
}

impl BasicBlock {
    pub fn new() -> Self {
        Self { body: Vec::new() }
    }
}

pub struct Function {
    pub body: Vec<BasicBlock>,
}

impl Function {
    pub fn new() -> Self {
        Self { body: Vec::new() }
    }
}

pub enum NameVariant {
    Name(String),
    Id(usize),
}

pub struct Context {
    funcs: Vec<Function>,

    inst_names: HashMap<InstId, NameVariant>,
    bb_names: HashMap<BasicBlockId, NameVariant>,
    func_names: HashMap<FunctionId, NameVariant>,
}

impl Context {
    pub fn new() -> Self {
        Self {
            funcs: Vec::new(),
            inst_names: HashMap::new(),
            bb_names: HashMap::new(),
            func_names: HashMap::new(),
        }
    }

    pub fn create_bb(&mut self, func_id: FunctionId) -> Option<BasicBlockId> {
        self.create_bb_with_name(func_id, "")
    }

    pub fn create_bb_with_name(
        &mut self,
        func_id: FunctionId,
        name: impl Into<String>,
    ) -> Option<BasicBlockId> {
        let idx = func_id.0;
        let id = BasicBlockId(self.funcs[idx].body.len());
        self.funcs[idx].body.push(BasicBlock::new());

        self.bb_names
            .insert(id, NameVariant::Name(name.into().clone()));

        Some(id)
    }

    pub fn create_function(&mut self) -> Option<FunctionId> {
        self.create_function_with_name("")
    }

    pub fn create_function_with_name(&mut self, name: impl Into<String>) -> Option<FunctionId> {
        let id = FunctionId(self.funcs.len());
        self.funcs.push(Function::new());

        self.func_names
            .insert(id, NameVariant::Name(name.into().clone()));

        Some(id)
    }
}
