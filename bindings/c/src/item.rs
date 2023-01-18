use std::{
    ffi::{c_char, CStr},
    mem::MaybeUninit,
    os::raw::c_void,
};

#[derive(Debug, PartialEq, PartialOrd)]
#[repr(transparent)]
pub struct ChecksItemWrapper(*const CChecksItem);

impl ChecksItemWrapper {
    pub(crate) fn new(item: *const CChecksItem) -> Self {
        Self(item)
    }

    pub(crate) fn ptr(&self) -> *const CChecksItem {
        self.0
    }
}

impl std::fmt::Display for ChecksItemWrapper {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        if self.0.is_null() {
            // TODO: Should we do a null check?
            panic!()
        }

        write!(f, "{}", unsafe { &*self.0 })
    }
}

/// The item is a wrapper to make a result item more user interface friendly.
///
/// Result items represent the objects that caused a result. For example, if a
/// check failed because the bones in a character rig are not properly named,
/// then the items would contain the bones that are named incorrectly.
///
/// The item wrapper makes the use of items user interface friendly because it
/// implements item sorting and a string representation of the item.
///
/// # Safety
///
/// It is assumed that the value the item contains is owned by the item wrapper.
#[repr(C)]
pub struct CChecksItem {
    /// A type hint can be used to add a hint to a system that the given type
    /// represents something else. For example, the value could be a string, but
    /// this is a scene path.
    ///
    /// A user interface could use this hint to select the item in the
    /// application.
    ///
    /// # Safety
    ///
    /// The string passed from the type hint function is owned by the item, or
    /// is static. A null pointer represents no type hint. The function must
    /// also contain type information needed by the `value_fn` for casting the
    /// void pointer to the correct type.
    pub type_hint_fn: extern "C" fn(&Self) -> *const c_char,

    /// The value that is wrapped.
    ///
    /// # Safety
    ///
    /// The value is assumed to be owned by the item wrapper. Also, the
    /// type_hint_fn must contain type information needed to cast the void
    /// pointer to the correct type.
    pub value_fn: extern "C" fn(&Self) -> *const c_void,
    /// The clone function will create a full copy of the item and its value.
    ///
    /// # Safety
    ///
    /// The items should only be read-only during their lifetime (excluding when
    /// they are deleted). So, if a value is going to be shared among items,
    /// then it should do so behind reference counters. Or, have the destroy
    /// function not actually modify/destroy the data, and leave that up to a
    /// process outside of the validation library.
    pub clone_fn: extern "C" fn(&Self, &mut Self),
    /// Destroy the owned data.
    ///
    /// # Safety
    ///
    /// The destroy function should be called once at most.
    pub destroy_fn: extern "C" fn(&mut Self) -> (),
    /// The debug function is used to create a string for debugging issues.
    ///
    /// # Safety
    ///
    /// The string's ownership is handed over to the caller, so it will not
    /// release the memory when finished. Also, do not modify or destroy the
    /// memory outside of the context in which the memory was created. For
    /// example, if the string was created with `malloc`, it should be deleted
    /// with `free`.
    pub debug_fn: extern "C" fn(&Self) -> crate::CChecksString,
    /// The display function is used to create a string for displaying to a
    /// user.
    ///
    /// # Safety
    ///
    /// The string's ownership is handed over to the caller, so it will not
    /// release the memory when finished. Also, do not modify or destroy the
    /// memory outside of the context in which the memory was created. For
    /// example, if the string was created with `malloc`, it should be deleted
    /// with `free`
    pub display_fn: extern "C" fn(&Self) -> crate::CChecksString,
    /// The order function is used to order items in user interfaces.
    pub lt_fn: extern "C" fn(&Self, &Self) -> bool,
    /// The compare function is used to order items in user interfaces.
    pub eq_fn: extern "C" fn(&Self, &Self) -> bool,
}

impl std::clone::Clone for CChecksItem {
    fn clone(&self) -> Self {
        let mut item = MaybeUninit::<Self>::uninit();

        unsafe {
            (self.clone_fn)(self, &mut *item.as_mut_ptr());

            item.assume_init()
        }
    }
}

impl Drop for CChecksItem {
    fn drop(&mut self) {
        (self.destroy_fn)(self)
    }
}

impl std::fmt::Display for CChecksItem {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let c_display = (self.display_fn)(self);

        if c_display.string.is_null() {
            return Err(std::fmt::Error);
        }

        unsafe {
            let c_str = CStr::from_ptr(c_display.string);
            let result = match c_str.to_str() {
                Ok(s) => write!(f, "{}", s),
                Err(_) => Err(std::fmt::Error),
            };

            result
        }
    }
}

impl std::fmt::Debug for CChecksItem {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let c_display = (self.debug_fn)(self);

        if c_display.string.is_null() {
            return Err(std::fmt::Error);
        }

        unsafe {
            let c_str = CStr::from_ptr(c_display.string);
            let result = match c_str.to_str() {
                Ok(s) => write!(f, "{}", s),
                Err(_) => Err(std::fmt::Error),
            };

            result
        }
    }
}

impl std::cmp::PartialEq for CChecksItem {
    fn eq(&self, other: &Self) -> bool {
        (self.eq_fn)(self, other)
    }
}

impl std::cmp::PartialOrd for CChecksItem {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        if (self.lt_fn)(self, other) {
            Some(std::cmp::Ordering::Less)
        } else if (self.eq_fn)(self, other) {
            Some(std::cmp::Ordering::Equal)
        } else {
            Some(std::cmp::Ordering::Greater)
        }
    }
}

impl checks::Item for CChecksItem {
    type Value = *const c_void;

    fn value(&self) -> Self::Value {
        (self.value_fn)(self)
    }
}

impl checks::Item for ChecksItemWrapper {
    type Value = *const c_void;

    fn value(&self) -> Self::Value {
        let item = unsafe {
            if self.0.is_null() {
                // TODO: Should panic?
                panic!()
            }
            &(*self.0)
        };
        (item.value_fn)(item)
    }
}

/// A type hint can be used to add a hint to a system that the given type
/// represents something else. For example, the value could be a string, but
/// this is a scene path.
///
/// A user interface could use this hint to select the item in the application.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub extern "C" fn cchecks_item_type_hint(item: &CChecksItem) -> *const c_char {
    (item.type_hint_fn)(item)
}

/// The value that is wrapped.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub extern "C" fn cchecks_item_value(item: &CChecksItem) -> *const c_void {
    (item.value_fn)(item)
}

/// Create a copy of the value contained by the item.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub extern "C" fn cchecks_item_clone(item: &CChecksItem, new_item: &mut CChecksItem) {
    (item.clone_fn)(item, new_item)
}

/// Destroy an item and its contents.
///
/// # Safety
///
/// The item pointer must not be null, and the item must not be deleted multiple
/// times (AKA: double free).
#[no_mangle]
pub extern "C" fn cchecks_item_destroy(item: &mut CChecksItem) {
    (item.destroy_fn)(item)
}

/// Create a debug string for the item.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub extern "C" fn cchecks_item_debug(item: &CChecksItem) -> crate::CChecksString {
    let result = format!("{:?}", item);

    crate::CChecksString::new(result)
}

/// Create a display string for the item for users.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub extern "C" fn cchecks_item_display(item: &CChecksItem) -> crate::CChecksString {
    let result = format!("{}", item);
    crate::CChecksString::new(result)
}

/// Return if the item is should be before or after the other item.
///
/// This is used for sorting items in user interfaces.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub extern "C" fn cchecks_item_lt(item: &CChecksItem, other: &CChecksItem) -> bool {
    item < other
}

/// Return if the item is is equal to the other item.
///
/// This is used for sorting items in user interfaces.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub extern "C" fn cchecks_item_eq(item: &CChecksItem, other: &CChecksItem) -> bool {
    item == other
}
